#if __has_include(<Arduino.h>)
#include <Arduino.h>
#else
#include <stddef.h>
#endif
// the handle to refer to a playing voice
typedef void* voice_handle_t;
// called when the sound output should be disabled
typedef void (*on_sound_disable_callback)(void* state);
// called when the sound output should be enabled
typedef void (*on_sound_enable_callback)(void* state);
// called when there's sound data to send to the output
typedef void (*on_flush_callback)(const void* buffer, size_t buffer_size, void* state);
// called to read a byte off a stream
typedef int (*on_read_stream_callback)(void* state);
// called to seek a stream
typedef void (*on_seek_stream_callback)(unsigned long long pos, void* state);
// represents a polyphonic player capable of playing wavs or various waveforms
class player final {
    voice_handle_t m_first;
    void* m_buffer;
    size_t m_frame_count;
    unsigned int m_sample_rate;
    unsigned int m_channel_count;
    unsigned int m_bit_depth;
    unsigned int m_sample_max;
    bool m_sound_enabled;
    on_sound_disable_callback m_on_sound_disable_cb;
    void* m_on_sound_disable_state;
    on_sound_enable_callback m_on_sound_enable_cb;
    void* m_on_sound_enable_state;
    on_flush_callback m_on_flush_cb;
    void* m_on_flush_state;
    player(const player& rhs)=delete;
    player& operator=(const player& rhs)=delete;
    void do_move(player& rhs);
    static void task(void* state);
    bool realloc_buffer();
public:
    // construct the player with the specified arguments
    player(unsigned int sample_rate = 44100, unsigned short channels = 2, unsigned short bit_depth = 16, size_t frame_count = 512);
    player(player&& rhs);
    ~player();
    player& operator=(player&& rhs);
    // indicates if the player has been initialized
    bool initialized() const;
    // initializes the player
    bool initialize();
    // deinitializes the player
    void deinitialize();
    // plays a sine wave at the specified frequency and amplitude
    voice_handle_t sin(float frequency, float amplitude = .8);
    // plays a square wave at the specified frequency and amplitude
    voice_handle_t sqr(float frequency, float amplitude = .8);
    // plays a sawtooth wave at the specified frequency and amplitude
    voice_handle_t saw(float frequency, float amplitude = .8);
    // plays a triangle wave at the specified frequency and amplitude
    voice_handle_t tri(float frequency, float amplitude = .8);
    // plays RIFF PCM wav data at the specified amplitude, optionally looping
    voice_handle_t wav(on_read_stream_callback on_read_stream, void* on_read_stream_state, float amplitude = .8, bool loop = false,on_seek_stream_callback on_seek_stream = nullptr, void* on_seek_stream_state=nullptr);
    // stops a playing voice, or all playing voices
    bool stop(voice_handle_t handle = nullptr);
    // set the sound disable callback
    void on_sound_disable(on_sound_disable_callback cb, void* state=nullptr);
    // set the sound enable callback
    void on_sound_enable(on_sound_enable_callback cb, void* state=nullptr);
    // set the flush callback (always necessary)
    void on_flush(on_flush_callback cb, void* state=nullptr);
    // a frame is every sample for every channel on a given a tick. A stereo frame would have two samples.
    // this is the count of frames in the mixing buffer
    size_t frame_count() const;
    // assign a new frame count
    bool frame_count(size_t value);
    // get the sample rate
    unsigned int sample_rate() const;
    // set the sample rate
    bool sample_rate(unsigned int value);
    // get the number of channels
    unsigned short channel_count() const;
    // set the number of channels
    bool channel_count(unsigned short value);
    // get the bit depth
    unsigned short bit_depth() const;
    // set the bit depth
    bool bit_depth(unsigned short value);
    // give a timeslice to the player to update itself
    void update();
    // gets the size of each frame in bytes
    constexpr static size_t frame_size(unsigned short channel_count, unsigned short bit_depth) {
        return channel_count * (bit_depth/8);
    }
};