#include <byteswap.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define FRAME_SIZE_RAW 2048
#define FRAME_SIZE 4096
#define CLIPINFO_OFFSET 0x00C800
#define FRAME_OFFSET    0x138C00
#define MAX_CLIPS 4096
void print_char(int ch);

typedef struct {
    int frames;
    int playframes;
    int frame[256];
    int time[256];
} clip;

typedef struct {
    uint16_t id;
    uint8_t frames;
    uint16_t m1;
    uint16_t m2;
    uint32_t l1;
    uint8_t m[18];
    char name[40];
    int clip_offset;
    int frame_offset;
} clip_info;

void read_clip_header(FILE *f, clip *c) {
//    printf("pos: %d\n", ftell(f));
    uint8_t frame[512];
    int highframe = 0;
    c->playframes = 0;
    c->frames = 0;
    fread(frame, 1, 512, f);
    for(int i = 0; i<256; i++) {
        if(!frame[i*2] && !frame[i*2+1])
            break;
//        printf("Frame %d, time %d\n", frame[0], frame[1]);
        if(frame[i*2] > highframe) 
            highframe = frame[i*2];
        c->playframes++;
        c->frame[i] = frame[i*2];
        c->time[i] = frame[i*2+1];
    }
    c->frames = highframe;
#if 0
//    printf("Frames: %d\nPlayframes: %d\n", c->frames, c->playframes);
    for(int i = 0; i < c->playframes; i++) {
        printf("\t%d %d\n", c->frame[i], c->time[i]);
//        printf("%d\n",c->time[i]);
    }
//    sleep(1);
#endif
}

void write_clip_info(FILE *f, clip *c, clip_info *ci) {
    fprintf(f, "{\n\"name\": \"%s\",\n\"frames\": %d,\n\"animation\": [", ci->name, ci->frames);
    for(int i = 0; i<c->playframes; i++) {
        if(i) {
            fprintf(f, ",");
        }
        fprintf(f, "%d", c->frame[i]);
    }
    fprintf(f, "],\n\"time\": [");
    for(int i = 0; i<c->playframes; i++) {
        if(i) {
            fprintf(f, ",");
        }
        fprintf(f, "%d", c->time[i]);
    }
    fprintf(f, "],\n\"mystery\": [");
    for(int i = 0; i<18; i++) {
        if(i) {
            fprintf(f, ",");
        }
        fprintf(f, "%d", ci->m[i]);
    }
    fprintf(f, "]\n},\n");
}

void write_animation(FILE *f, char *name, uint8_t *frame, int frames) {
    fprintf(f, "{\n\"name\": \"%s\",\n\"frames\":[\n", name);
    for(int i = 0; i<frames;i++){
        if(i)
            fprintf(f, ",\n");
        fprintf(f, "  [");
        for(int j = 0; j<4096;j++){
            if(j)
                fprintf(f, ",");
            fprintf(f, "%d", frame[i*4096+j]);
        }
        fprintf(f, "]");
    }
    fprintf(f, "\n]\n}\n");
}

void print_frame(uint8_t *frame, uint8_t sleep, char *name){
    printf("Frame! %dms\t%s\n", sleep, name);
    for (int col = 0; col < 32; col++) {
        for (int row = 0; row < 128; row++) {
           print_char(frame[col*128+row]);
        }
        printf("\n");
    }
    if(sleep > 100) {
        usleep((sleep)*4000);
    } else {
        usleep((sleep)*1500);
    }
}

int main() {
    uint8_t frame_raw[FRAME_SIZE_RAW];
    uint8_t frame[257][4096];
    uint8_t buf[0x200];
    FILE *in;
    size_t result;
    clip_info *clips[MAX_CLIPS];
    in = fopen("test.img", "r");
    fseek(in, CLIPINFO_OFFSET, SEEK_SET);
    int clip_infos = 0;
    int frames = 0;
    // Read clip infos
    for(;clip_infos < MAX_CLIPS; clip_infos++) {
        result = fread(buf, 1, 0x200, in);
        if(result != 0x200)
            break;
        if(buf[0] == 0 && buf[1] == 0)
            break;
        clip_info *ci = (clip_info *) malloc(sizeof(clip_info));
        clips[clip_infos] = ci;
//        memcpy(&ci->id, buf, 2);
        ci->id = bswap_16(*((uint16_t *) buf));
        ci->m1 = bswap_16(*((uint16_t *) buf+1));
        ci->m2 = bswap_16(*((uint16_t *) (buf+5)));
        ci->l1 = bswap_32(*((uint32_t *) (buf+5)));
        ci->clip_offset = clip_infos;
        ci->frame_offset = frames;
        ci->frames = buf[3];
        frames += ci->frames;
        memcpy(ci->m, buf+2, 18);
        memcpy(ci->name, buf + 20, 40);
//        for(int i = 0; i<18; i++)
//            printf("%d\t", ci->m[i]);

//        printf("\n");
//        printf("clip id: %d %s (%d frames offset: %d clips %d frames)\n", ci->id, ci->name, ci->frames, ci->clip_offset, ci->frame_offset);
    }
    fseek(in, FRAME_OFFSET, SEEK_SET);
    int frame_cnt = 0;
    for(int cl = 0; cl < clip_infos; cl++) {
        clip c;
        read_clip_header(in, &c);
        printf("Frames: %d, playframes: %d %s\n", c.frames, c.playframes, clips[cl]->name);
//        write_clip_info(out, &c, clips[cl]);
        // Read
//        for(int i = 0; i < c.frames; i++) {
        for(int i = 0; i < clips[cl]->frames; i++) {
            result = fread(frame_raw, 1, FRAME_SIZE_RAW, in);
            if (result != FRAME_SIZE_RAW){
                printf("EOF\n");
                return 0;
            }
            for (int j = 0; j < FRAME_SIZE_RAW; j++) {
                frame[i][j*2 + 0] = (frame_raw[j] & 0xF0 ) >> 4;
                frame[i][j*2 + 1] = (frame_raw[j] & 0x0F ) >> 0;
            }
        }
//        if(strncmp("AVENG", clips[cl]->name,5)) {
//        if(clips[cl]->m[12] != 1) {
//            continue;
//        }
        FILE *out;
        char file[100];
        sprintf(file, "json/%s.json", clips[cl]->name);
        out = fopen(file, "w+");
        write_animation(out, clips[cl]->name, frame, clips[cl]->frames);
        fclose(out);
        printf("id: %d\n",clips[cl]->id);
        for(int i = 0; i < c.playframes; i++) {
//            print_frame(frame[c.frame[i]-1], c.time[i], clips[cl]->name);
        }
    }
}

void print_char(int ch) {
    // 69 characters..
    static char ramp[] = "$@B%8&WM#*oahkbdpqwmZO0QLCJUYXzcvunxrjft/\\|()1{}[]?-_+~<>i!lI;:,\"^`'. ";
    int idx = ((15.0-ch)*4.6);
    if(ch == 10) {
        printf("  ");
    } else {
        printf("%c", ramp[idx]);
        printf("%c", ramp[idx]);
    }
    return;
   
    if(ch == 0) {
        printf("  ");
    } else if (ch == 1) {
        printf("..");
    } else if (ch == 2) {
        printf("::");
    } else if (ch == 3) {
        printf("--");
    } else if (ch == 4) {
        printf("==");
    } else if (ch == 5) {
        printf("++");
    } else if (ch == 6) {
        printf("**");
    } else if (ch == 7) {
        printf("##");
    } else if (ch == 8) {
        printf("88");
    } else if (ch == 9) {
        printf("99");
    } else if (ch == 10) {
        printf("  "); // Transparent?
    } else if (ch == 11) {
        printf("11");
    } else if (ch == 12) {
        printf("22");
    } else if (ch == 13) {
        printf("33");
    } else if (ch == 14) {
        printf("44");
    } else if (ch == 15) {
        printf("55");
    } else if (ch == 16) {
        printf("66");
        exit(0);
    } else {
        printf("Error! %d\n");
    }
} 
