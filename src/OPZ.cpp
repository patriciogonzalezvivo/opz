#include <stdio.h>  /* sprintf() */
#include <stdlib.h> /* malloc() */
#include <string.h>
#include <iostream>
#include <vector>

#include "OPZ.h"

namespace T3 {

// TOOLS
#define BIT(x,b) ((x & (1<<b)))
#define CAST_MESSAGE(TYP, NAM) const TYP & NAM = (const TYP &)data[0]
#define BYTE_TO_BINARY_PATTERN "%c %c %c %c  %c %c %c %c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

// https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol
std::vector<unsigned char> initial_message = {
    SYSEX_HEAD, 0x7E, 0x7F, 0x06, 0x01, SYSEX_END
};

// 0x00 Master Heartbeat (https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#00-master-heartbeat)
std::vector<unsigned char> master_heartbeat = { 
    // Midi::SYSTEM_EXCLUSIVE, OPZ_VENDOR_ID[0], OPZ_VENDOR_ID[1], OPZ_VENDOR_ID[2], OPZ_MAX_PROTOCOL_VERSION, 0x00, 0x03, 0x2D, 0x0E, 0x05, Midi::END_OF_SYSEX // prior to version 1.2.5
    SYSEX_HEAD, OPZ_VENDOR_ID[0], OPZ_VENDOR_ID[1], OPZ_VENDOR_ID[2], OPZ_MAX_PROTOCOL_VERSION, 0x00, 0x01, 0x4E, 0x2E, 0x06, SYSEX_END // version 1.2.5
}; 

std::string key_name[] = {
    "KEY_POWER",
    "KEY_PROJECT", "KEY_MIXER", "KEY_TEMPO", "KEY_SCREEN",
    "KEY_TRACK", "KEY_KICK", "KEY_SNARE", "KEY_PERC", "KEY_SAMPLE", "KEY_BASS", "KEY_LEAD", "KEY_ARP", "KEY_CHORD", "KEY_FX1", "KEY_FX2", "KEY_TAPE", "KEY_MASTER", "KEY_PERFORM", "KEY_MODULE", "KEY_LIGHT", "KEY_MOTION",
    "KEY_RECORD", "KEY_PLAY", "KEY_STOP",
    "KEY_MINUS", "KEY_PLUS", "KEY_SHIFT"
};

// https://teenage.engineering/guides/op-z/parameter-pages
std::string page_name[] = { 
    "ONE", "TWO", "THREE", "FOUR" 
};

// https://teenage.engineering/guides/op-z/tracks
std::string track_name[] = { 
    "KICK", "SNARE", "PERC", "SAMPLE", "BASS", "LEAD", "ARP", "CHORD", "FX1", "FX2", "TAPE", "MASTER", "PERFORM", "MODULE", "LIGHT", "MOTION", "UNKNOWN" 
};

std::string pattern_parameter_name[] = { 
    "KICK_PLUG",    "KICK_PLUG1",   "KICK_PLUG2",   "KICK_PLUG3",   "KICK_STEP_COUNT",  "KICK_UNKNOWN", "KICK_STEP_LENGTH", "KICK_QUANTIZE",    "KICK_NOTE_STYLE",  "KICK_NOTE_LENGTH", "KICK_BYTE1",   "KICK_BYTE2",
    "SNARE_PLUG",   "SNARE_PLUG1",  "SNARE_PLUG2",  "SNARE_PLUG3",  "SNARE_STEP_COUNT", "SNARE_UNKNOWN","SNARE_STEP_LENGTH","SNARE_QUANTIZE",   "SNARE_NOTE_STYLE", "SNARE_NOTE_LENGTH","SNARE_BYTE1", "SNARE_BYTE2",
    "PERC_PLUG",    "PERC_PLUG1",   "PERC_PLUG2",   "PERC_PLUG3",   "PERC_STEP_COUNT",  "PERC_UNKNOWN", "PERC_STEP_LENGTH", "PERC_QUANTIZE",    "PERC_NOTE_STYLE",  "PERC_NOTE_LENGTH","PERC_BYTE1",    "PERC_BYTE2",
    "SAMPLE_PLUG",  "SAMPLE_PLUG1", "SAMPLE_PLUG2", "SAMPLE_PLUG3", "SAMPLE_STEP_COUNT",  "SAMPLE_UNKNOWN", "SAMPLE_STEP_LENGTH", "SAMPLE_QUANTIZE", "SAMPLE_NOTE_STYLE", "SAMPLE_NOTE_LENGTH", "SAMPLE_BYTE1",   "SAMPLE_BYTE2",
    "BASS_PLUG",    "BASS_PLUG1",   "BASS_PLUG2",   "BASS_PLUG3",   "BASS_STEP_COUNT",    "BASS_UNKNOWN",   "BASS_STEP_LENGTH",   "BASS_QUANTIZE",  "BASS_NOTE_STYLE",    "BASS_NOTE_LENGTH",   "BASS_BYTE1",     "BASS_BYTE2",
    "LEAD_PLUG",    "LEAD_PLUG1",   "LEAD_PLUG2",   "LEAD_PLUG3", "LEAD_STEP_COUNT",    "LEAD_UNKNOWN",   "LEAD_STEP_LENGTH",   "LEAD_QUANTIZE",  "LEAD_NOTE_STYLE",    "LEAD_NOTE_LENGTH",   "LEAD_BYTE1",     "LEAD_BYTE2",
    "ARC_PLUG",     "ARC_PLUG1",    "ARC_PLUG2",    "ARC_PLUG3",      "ARC_STEP_COUNT",     "ARC_UNKNOWN",    "ARC_STEP_LENGTH",    "ARC_QUANTIZE",   "ARC_NOTE_STYLE",     "ARC_NOTE_LENGTH",    "ARC_BYTE1",      "ARC_BYTE2",
    "CHORD_PLUG",   "CHORD_PLUG1",  "CHORD_PLUG2",  "CHORD_PLUG3",    "CHORD_STEP_COUNT",   "CHORD_UNKNOWN",  "CHORD_STEP_LENGTH",  "CHORD_QUANTIZE", "CHORD_NOTE_STYLE",   "CHORD_NOTE_LENGTH",  "CHORD_BYTE1",    "CHORD_BYTE2",
    "FX1_PLUG",     "FX1_PLUG1",    "FX1_PLUG2",    "FX1_PLUG3",      "FX1_STEP_COUNT",     "FX1_UNKNOWN",    "FX1_STEP_LENGTH",    "FX1_QUANTIZE",   "FX1_NOTE_STYLE",     "FX1_NOTE_LENGTH",    "FX1_BYTE1",      "FX1_BYTE2",
    "FX2_PLUG",     "FX2_PLUG1",    "FX2_PLUG2",    "FX2_PLUG3",      "FX2_STEP_COUNT",     "FX2_UNKNOWN",    "FX2_STEP_LENGTH",    "FX2_QUANTIZE",   "FX2_NOTE_STYLE",     "FX2_NOTE_LENGTH",    "FX2_BYTE1",      "FX2_BYTE2",
    "TAPE_PLUG",    "TAPE_PLUG1",   "TAPE_PLUG2",   "TAPE_PLUG3",     "TAPE_STEP_COUNT",    "TAPE_UNKNOWN",   "TAPE_STEP_LENGTH",   "TAPE_QUANTIZE",  "TAPE_NOTE_STYLE",    "TAPE_NOTE_LENGTH",   "TAPE_BYTE1",     "TAPE_BYTE2",
    "MASTER_PLUG",  "MASTER_PLUG1", "MASTER_PLUG2", "MASTER_PLUG3",   "MASTER_STEP_COUNT",  "MASTER_UNKNOWN", "MASTER_STEP_LENGTH", "MASTER_QUANTIZE","MASTER_NOTE_STYLE",  "MASTER_NOTE_LENGTH", "MASTER_BYTE1",   "MASTER_BYTE2",
    "PERFORM_PLUG", "PERFORM_PLUG1","PERFORM_PLUG2","PERFORM_PLUG3",  "PERFORM_STEP_COUNT", "PERFORM_UNKNOWN","PERFORM_STEP_LENGTH","PERFORM_QUANTIZE","PERFORM_NOTE_STYLE","PERFORM_NOTE_LENGTH","PERFORM_BYTE1",  "PERFORM_BYTE2",
    "MODULE_PLUG",  "MODULE_PLUG1", "MODULE_PLUG2", "MODULE_PLUG3",   "MODULE_STEP_COUNT",  "MODULE_UNKNOWN", "MODULE_STEP_LENGTH", "MODULE_QUANTIZE","MODULE_NOTE_STYLE",  "MODULE_NOTE_LENGTH", "MODULE_BYTE1",   "MODULE_BYTE2",
    "LIGHT_PLUG",   "LIGHT_PLUG1",  "LIGHT_PLUG2",  "LIGHT_PLUG3",    "LIGHT_STEP_COUNT",   "LIGHT_UNKNOWN",  "LIGHT_STEP_LENGTH",  "LIGHT_QUANTIZE", "LIGHT_NOTE_STYLE",   "LIGHT_NOTE_LENGTH",  "LIGHT_BYTE1",    "LIGHT_BYTE2",
    "MOTION_PLUG",  "MOTION_PLUG1", "MOTION_PLUG2",   "MOTION_PLUG3",   "MOTION_STEP_COUNT",  "MOTION_UNKNOWN", "MOTION_STEP_LENGTH", "MOTION_QUANTIZE","MOTION_NOTE_STYLE",  "MOTION_NOTE_LENGTH", "MOTION_BYTE1",   "MOTION_BYTE2"
};

std::string track_parameter_name[] = { 
    "SOUND_PARAM1",     "SOUND_PARAM2",     "SOUND_FILTER",     "SOUND_RESONANCE", 
    "ENVELOPE_ATTACK",  "ENVELOPE_DECAY",   "ENVELOPE_SUSTAIN", "ENVELOPE_RELEASE",
    "SOUND_FX1",        "SOUND_FX2",        "SOUND_PAN",        "SOUND_LEVEL",
    "LFO_DEPTH",        "LFO_SPEED",        "LFO_VALUE",        "LFO_SHAPE",
    "NOTE_LENGHT",      "NOTE_STYLE",       "QUANTIZE",         "PORTAMENTO"
};

// print array of unsigend chars as HEX pairs
char *printHex(unsigned char *cp, size_t n) {
    char *s = (char*)malloc(3*n + 1);

    if (s == NULL)
        return s;

    for (size_t k = 0; k < n; ++k)
        sprintf(s + 3*(k), "%02X ", cp[k]);

    s[3*n] = '\0';
    return s;
}

// encode raw 8bit to 7bits  
size_t encodeSysEx(const unsigned char* inData, unsigned char* outSysEx, unsigned inLength, bool inFlipHeaderBits) {
    size_t outLength    = 0;     // Num bytes in output array.
    unsigned char count = 0;     // Num 7bytes in a block.
    outSysEx[0]         = 0;

    for (unsigned i = 0; i < inLength; ++i) {
        const unsigned char data = inData[i];
        const unsigned char msb  = data >> 7;
        const unsigned char body = data & 0x7f;

        outSysEx[0] |= (msb << (inFlipHeaderBits ? count : (6 - count)));
        outSysEx[1 + count] = body;

        if (count++ == 6) {
            outSysEx   += 8;
            outLength  += 8;
            outSysEx[0] = 0;
            count       = 0;
        }
    }
    return outLength + count + (count != 0 ? 1 : 0);
}

// decode 7bit to raw 8bits
size_t decodeSysEx(const unsigned char* inData, unsigned char* outData, size_t inLength, bool inFlipHeaderBits) {
    size_t count  = 0;
    unsigned char msbStorage = 0;
    unsigned char byteIndex  = 0;

    for (size_t i = 0; i < inLength; ++i) {
        if ((i % 8) == 0) {
            msbStorage = inData[i];
            byteIndex  = 6;
        }
        else {
            const unsigned char body     = inData[i];
            const unsigned char shift    = inFlipHeaderBits ? 6 - byteIndex : byteIndex;
            const unsigned char msb      = u_int8_t(((msbStorage >> shift) & 1) << 7);
            byteIndex--;
            outData[count++] = msb | body;
        }
    }
    return count;
}

OPZ::OPZ():
verbose(0),
m_volume(0.0f), 
m_active_track(KICK), 
m_active_page(PAGE_ONE), 
m_microphone_mode(0),
m_play(false),
m_key_enable(false) {
}

std::string& OPZ::toString( key_id _id ) { return key_name[_id]; }
std::string& OPZ::toString( page_id  _id ) { return page_name[_id]; }
std::string& OPZ::toString( track_id _id ) { return track_name[_id]; }
std::string& OPZ::toString( track_parameter_id _id ) { return track_parameter_name[_id]; } 
std::string& OPZ::toString( pattern_parameter_id _id ) { return pattern_parameter_name[_id]; } 
const std::vector<unsigned char>* OPZ::getInitMsg() { return &initial_message; }
const std::vector<unsigned char>* OPZ::getHeartBeat() { return &master_heartbeat; }

void OPZ::process_message(double _deltatime, std::vector<unsigned char>* _message, void* _userData) {
    OPZ *device = static_cast<OPZ*>(_userData);

    if (_message->at(0) == SYSEX_HEAD)
        device->process_sysex(_message);
    else
        device->process_event(_message);
}

void OPZ::process_sysex(std::vector<unsigned char>* _message){

    // if (verbose > 1)
    //     std::cout << printHex(&_message->at(0), _message->size()) << "(" << _message->size() << " bytes)" << std::endl;

    const sysex_header &header = (const sysex_header&)_message->at(0);
    if (memcmp(OPZ_VENDOR_ID, header.vendor_id, sizeof(OPZ_VENDOR_ID)) != 0){
        printf("Vendor ID %02X:%02X:%02X is not the expected ID %02X:%02X:%02X\n", header.vendor_id[0],header.vendor_id[1],header.vendor_id[2], OPZ_VENDOR_ID[0],OPZ_VENDOR_ID[1],OPZ_VENDOR_ID[2]);
        return;
    }

    if ((header.protocol_version == 0) || (header.protocol_version > OPZ_MAX_PROTOCOL_VERSION)){
        printf("Unexpected protocol version %02X, was expecting > 0 and <= %02X\n", header.protocol_version, OPZ_MAX_PROTOCOL_VERSION);
        return;
    }

    // TODO: check that first bit of data is 'F000207601'

    // substract the header and end of SYSEX message
    size_t data_length = _message->size()-sizeof(sysex_header)-1;
    unsigned char *data = new unsigned char[data_length];

    // decode 7bit into raw 8bit
    size_t length = decodeSysEx(&_message->at(sizeof(sysex_header)), data, data_length, true);
    
    switch (header.parm_id) {
        case 0x01: {
            // // Universal response ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#01-universal-response )
            // if (verbose)
            //     printf("Msg %02X (Universal response)\n", header.parm_id);

            // if (verbose > 1)
            //     std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
        } break;

        case 0x02: {
            // Track Setting ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#02-track-settings )
            if (verbose)
                printf("Msg %02X (Track Setting)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;

            CAST_MESSAGE(track_change, ti);

            if (verbose > 2) {
                std::cout << "   value type: " << toString((pattern_parameter_id)ti.value_type) << std::endl;
                std::cout << "   value:      " << ((int)ti.value / 255.0f) << std::endl;
            }

        } break;

        case 0x03: {
            // Keyboard Setting ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#03-keyboard-setting )
            if (verbose)
                printf("Msg %02X (Keyboard Setting)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;

            CAST_MESSAGE(track_keyboard, ki);
            m_active_track = (track_id)ki.track;

            memcpy(&(m_track_keyboard[m_active_track]), &ki, sizeof(track_keyboard));
            
            if (verbose > 2) {
                std::cout << "  active track:   " << toString(m_active_track) << std::endl;
                // printf(      "  active octave:  %02X\n", ki.octave);
                printf(      "  active octave:  %i\n", ki.octave);
            }
            
        } break;

        case 0x04: {
            if (verbose)
                printf("Msg %02X (IN/OUT?)\n", header.parm_id);

            if (verbose > 1) {
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
            }

            m_volume = data[1]/255.0;
            m_microphone_mode = data[3];

            if (verbose > 2) {
                printf(" ????????   %02x\n", data[0] );
                printf(" Volumen    %f\n",  m_volume);
                printf(" ????????   %02x\n", data[2] );
                printf(" Microphone %02x\n", m_microphone_mode);
            }

            if (m_key_enable)
                m_key(KEY_POWER, m_volume * 100);
        } break;

        case 0x06: {
            // Button States ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#06-button-states )
            if (verbose)
                printf("Msg %02X (Button States)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;

            CAST_MESSAGE(key_state, ki);
            memcpy(&(m_key_prev_state), &m_key_state, sizeof(m_key_state));
            memcpy(&(m_key_state), &ki, sizeof(m_key_state));
            m_active_page = (page_id)m_key_state.page;

            if (m_key_enable) {
                if (verbose > 2) {
                    std::cout << "  track: " << toString(m_active_track) << std::endl;
                    std::cout << "  page:  " << toString(m_active_page) << std::endl;

                    printf( "bit1 1-6:  %i %i %i %i %i %i\n", m_key_state.bit11, m_key_state.bit12, m_key_state.bit13, m_key_state.bit14, m_key_state.bit15, m_key_state.bit16);
                    printf( "page:      %i\n", m_key_state.page);
                    printf( "step:      %i\n", m_key_state.step);
                    printf( "shift:     %i\n", m_key_state.shift);
                    printf( "tempo:     %i\n", m_key_state.tempo);
                    printf( "mixer:     %i\n", m_key_state.mixer);
                    printf( "bit3 1-5:  %i %i %i %i %i\n", m_key_state.bit31, m_key_state.bit32, m_key_state.bit33, m_key_state.bit34, m_key_state.bit35);
                    printf( "screen:    %i\n", m_key_state.screen);
                    printf( "stop:      %i\n", m_key_state.stop);
                    printf( "record:    %i\n", m_key_state.record);
                    printf( "track:     %i\n", m_key_state.track);
                    printf( "project:   %i\n", m_key_state.project);
                    printf( "bit4 3-8:  %i %i %i %i %i %i\n", m_key_state.bit43, m_key_state.bit44, m_key_state.bit45, m_key_state.bit46, m_key_state.bit47, m_key_state.bit48);
                }

                if (m_key_state.project) m_key( KEY_PROJECT, 1);
                if (m_key_prev_state.project && !m_key_state.project ) m_key( KEY_PROJECT, 0);
                
                if (m_key_state.mixer) m_key( KEY_MIXER, 1);
                if (m_key_prev_state.mixer && !m_key_state.mixer) m_key( KEY_MIXER, 0);

                if (m_key_state.tempo) m_key( KEY_TEMPO, 1);
                if (m_key_prev_state.tempo && !m_key_state.tempo) m_key( KEY_TEMPO, 0);
                
                if (m_key_state.screen) m_key( KEY_SCREEN, 1);
                if (m_key_prev_state.screen && !m_key_state.screen) m_key( KEY_SCREEN, 0);

                if (m_key_state.track) m_key( KEY_TRACK, 1);
                if (m_key_prev_state.track && !m_key_state.track) m_key( KEY_TRACK, 0);

                if (m_key_state.step < 16) m_key( key_id( 6+(int)m_key_state.step ), 1);
                if (m_key_prev_state.step < 16 && m_key_state.step >= 16) m_key( key_id( 6+(int)m_key_prev_state.step ), 0);

                if (m_key_state.record) m_key( KEY_RECORD, 1);
                if (m_key_prev_state.record && !m_key_state.record) m_key( KEY_RECORD, 0);

                // TODO: KEY_PLAY

                if (m_key_state.stop) m_key( KEY_STOP, 1);
                if (m_key_prev_state.stop && !m_key_state.stop) m_key( KEY_STOP, 0);

                // TODO: KEY_MINUX / KEY_PLUS

                if (m_key_state.shift) m_key( KEY_SHIFT, 1);
                if (m_key_prev_state.shift && !m_key_state.shift) m_key( KEY_SHIFT, 0);
            }

        } break;

        case 0x07: {
            // Sequencer Settings ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#07-sequencer-settings )
            if (verbose)
                printf("Msg %02X (Sequencer Settings)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;

            CAST_MESSAGE(sequence_change, si);
            m_active_project = si.project;
            m_active_sequence = si.sequence; // project + pattern
            m_active_pattern = si.pattern;

            if (verbose > 2) {
                printf("    sequence:   %i\n", si.pattern + 1);
                printf("    unknown10:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[0]));
                printf("    unknown11:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[1]));
                printf("    unknown12:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[2]));
                printf("    unknown13:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[3]));
                printf("    unknown14:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[4]));
                printf("    unknown15:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[5]));
                printf("    unknown16:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[6]));
                printf("    unknown17:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[7]));
                printf("    unknown18:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[8]));
                printf("    unknown19:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[9]));
                printf("    unknown1A:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[10]));
                printf("    unknown1B:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[11]));
                printf("    unknown1C:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[12]));
                printf("    unknown1D:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[13]));
                printf("    unknown1E:  %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown1[14]));
                printf("    unknown2:   %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown2));
                printf("    pattern:    %02X\n", si.pattern);
                printf("    unknown3:   %c %c %c %c  %c %c %c %c\n", BYTE_TO_BINARY(si.unknown3));
                printf("    project:    %i\n", si.project + 1);
            }

        } break;

        case 0x09: {
            // Pattern ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#09-pattern )
            if (verbose)
                printf("Msg %02X (Pattern)\n", header.parm_id);
            
            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
            
        } break;

        case 0x0c: {
            // Global Data ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#0c-global-data )
            if (verbose)
                printf("Msg %02X (Global Data)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
            
        } break;

        case 0x0e: {
            // Sound preset ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#0e-sound-preset )
            if (verbose)
                printf("Msg %02X (Track Parameter)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "    (" << length << " bytes)" << std::endl;

            m_active_track = (track_id)data[0];

            const track_parameter & si = (const track_parameter &)data[1];
            memcpy(&(m_track_parameter[m_active_track]), &si, sizeof(track_parameter));

            if (verbose > 2) {
                std::cout << "     " << toString(m_active_track) << std::endl;
                printf( "   param1:     %i\n", si.param1);
                printf( "   param2:     %i\n", si.param2);
                printf( "   attack:     %i\n", si.attack);
                printf( "   decay:      %i\n", si.decay);
                printf( "   ustain:     %i\n", si.sustain);
                printf( "   release:    %i\n", si.release);
                printf( "   fx1:        %i\n", si.fx1);
                printf( "   fx2:        %i\n", si.fx2);
                printf( "   filter:     %i\n", si.filter);
                printf( "   resonance:  %i\n", si.resonance);
                printf( "   pan:        %i\n", si.pan);
                printf( "   level:      %i\n", si.level);
                printf( "   portamendo: %i\n", si.portamento);
                printf( "   lfo_depth:  %i\n", si.lfo_depth);
                printf( "   lfo_speed:  %i\n", si.lfo_speed);
                printf( "   lfo_value:  %i\n", si.lfo_value);
                printf( "   lfo_shape:  %i\n", si.lfo_shape);
                printf( "   note_style: %i\n", si.note_style);
            }
        } break;

        case 0x10: {
            // Compressed MIDI Config ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#10-zlib-compressed-midi-config )
            if (verbose)
                printf("Msg %02X (Compressed MIDI Config)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
        } break;

        case 0x12: {
            // Sound State ( https://github.com/hyphz/opzdoc/wiki/MIDI-Protocol#12-sound-state )
            if (verbose)
                printf("Msg %02X (Sound State)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
        } break;

        default: {
            if (verbose)
                printf("Msg %02X (unknown)\n", header.parm_id);

            if (verbose > 1)
                std::cout << "       " << printHex(data, length) << "(" << length << " bytes)" << std::endl;
        }
    }

}

void OPZ::process_event(std::vector<unsigned char>* _message) {

    if (_message->at(0) == START_SONG)
        m_play = true;
    else if (_message->at(0) == STOP_SONG)
        m_play = false;

    int bytes = 0;
    unsigned char status = 0;
    unsigned char channel = 0;

    int j = 0;
    if ((_message->at(0) & 0xf0) != 0xf0) {
        channel = _message->at(0) & 0x0f;
        channel += 1;
        status = _message->at(0) & 0xf0;
    }
    else {
        channel = 0;
        status = _message->at(0);
    }

    switch (status) {
        case CONTROLLER_CHANGE:
            bytes = 2;
            break;

        case NOTE_ON:
            bytes = 2;
            break;

        case NOTE_OFF:
            bytes = 2;
            break;

        case KEY_PRESSURE:
            bytes = 2;
            break;

        case PROGRAM_CHANGE:
            bytes = 1;
            break;

        case CHANNEL_PRESSURE:
            bytes = 1;
            break;

        case PITCH_BEND:
            bytes = 2;
            break;

        case SONG_POSITION:
            bytes = 2;
            break;

        case SONG_SELECT:
            bytes = 2;
            break;

        case TUNE_REQUEST:
            bytes = 2;
            break;

        case TIMING_TICK:
            bytes = 0;
            break;

        case START_SONG:
            bytes = 0;
            break;

        case CONTINUE_SONG:
            bytes = 0;
            break;

        case STOP_SONG:
            bytes = 0;
            break;

        default:
            bytes = 0;
            break;
    }

    if (status == NOTE_ON && _message->at(2) == 0)
        status = NOTE_OFF;

    // if (bytes >= 2) {
    //     std::cout << ", channel: " << channel;

    //     size_t key = _message->at(1);
    //     size_t target_value = _message->at(2);

    //     std::cout << ", key: " << _message->at(1);
    //     std::cout << ", value: " << _message->at(2);
    // }

    // std::cout << std::endl;
}

float OPZ::getTrackPageParameter(track_id _track, track_parameter_id _prop) {
    if (_prop == SOUND_PARAM1)
        return m_track_parameter[m_active_track].param1 / 255.0f;
    else if (_prop == SOUND_PARAM2)
        return m_track_parameter[m_active_track].param2 / 255.0f;
    else if (_prop == SOUND_FILTER)
        return m_track_parameter[m_active_track].filter / 255.0f; 
    else if (_prop == SOUND_RESONANCE)
        return m_track_parameter[m_active_track].resonance / 255.0f;

    // TODO
    //      - I got the names wrong
    else if (_prop == ENVELOPE_ATTACK) // S
        return m_track_parameter[m_active_track].attack / 255.0f;
    else if (_prop == ENVELOPE_DECAY) // A
        return m_track_parameter[m_active_track].decay  / 255.0f;
    else if (_prop == ENVELOPE_SUSTAIN) // H
        return m_track_parameter[m_active_track].sustain / 255.0f; 
    else if (_prop == ENVELOPE_RELEASE) // D
        return m_track_parameter[m_active_track].release / 255.0f; 

    else if (_prop == LFO_DEPTH)
        return m_track_parameter[m_active_track].lfo_depth / 255.0f;
    else if (_prop == LFO_SPEED) // RATE
        return m_track_parameter[m_active_track].lfo_speed / 255.0f;
    else if (_prop == LFO_VALUE) // DEST
        return m_track_parameter[m_active_track].lfo_value / 255.0f; 
    else if (_prop == LFO_SHAPE)
        return m_track_parameter[m_active_track].lfo_shape / 255.0f;

    else if (_prop == SOUND_FX1)
        return m_track_parameter[m_active_track].fx1 / 255.0f;
    else if (_prop == SOUND_FX2)
        return (m_track_parameter[m_active_track].fx2 / 255.0f) * 2.0 - 1.0f;
    else if (_prop == SOUND_PAN)
        return m_track_parameter[m_active_track].pan / 255.0f; 
    else if (_prop == SOUND_LEVEL)
        return m_track_parameter[m_active_track].level / 255.0f;

    else if (_prop = NOTE_STYLE)
        return m_track_parameter[m_active_track].note_style / 255.0f;

    else if (_prop == PORTAMENTO)
        return m_track_parameter[m_active_track].portamento / 255.0f;

    return 0.0f;
}

}