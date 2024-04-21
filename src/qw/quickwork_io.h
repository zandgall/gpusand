#ifndef QUICKWORK_IO_H
#define QUICKWORK_IO_H
namespace qwio {
    extern unsigned int load_shader(const char* path);
    extern unsigned int get_loaded_shader(const char* name);
    extern void load_shader_as(const char* path, const char* name);

    extern unsigned int load_texture(const char* path, int magFilter=0x2600, int minFilter=0x2700, int wrapS=0x812F, int wrapT=0x812F);
    extern unsigned int get_loaded_texture(const char* name);
    extern void load_texture_as(const char* path, const char* name, int magFilter=0x2600, int minFilter=0x2700, int wrapS=0x812F, int wrapT=0x812F);

    extern unsigned int load_compute(const char* path);
    extern unsigned int get_loaded_compute(const char* name);
    extern void load_compute_as(const char* path, const char* name);

    extern void load_font_as(const char* path, const char* name, int font_size=48, unsigned long glyphs_to_load=256UL);
}
#endif