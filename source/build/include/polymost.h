#ifndef polymost_h_
# define polymost_h_

#ifdef USE_OPENGL

#include "baselayer.h"  // glinfo
#include "glad/glad.h"
#include "hightile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct { uint8_t r, g, b, a; } coltype;
typedef struct { float r, g, b, a; } coltypef;

extern int32_t rendmode;
extern float gtang;
extern int polymost2d;
extern double gxyaspect;
extern float grhalfxdown10x;
extern float gcosang, gsinang, gcosang2, gsinang2;
extern float gchang, gshang, gctang, gstang, gvisibility;
extern float gvrcorrection;

struct glfiltermodes {
    const char *name;
    int32_t min,mag;
};
#define NUMGLFILTERMODES 6
extern struct glfiltermodes glfiltermodes[NUMGLFILTERMODES];

extern void Polymost_prepare_loadboard(void);

void polymost_outputGLDebugMessage(uint8_t severity, const char* format, ...);

//void phex(char v, char *s);
void polymost_setuptexture(const int32_t dameth, int filter);
void uploadtexture(int32_t doalloc, vec2_t siz, int32_t texfmt, coltype *pic, vec2_t tsiz, int32_t dameth);
void uploadtextureindexed(int32_t doalloc, vec2_t offset, vec2_t siz, intptr_t tile);
void uploadbasepalette(int32_t basepalnum);
void uploadpalswap(int32_t palookupnum);
void polymost_drawsprite(int32_t snum);
void polymost_drawmaskwall(int32_t damaskwallcnt);
void polymost_dorotatesprite(int32_t sx, int32_t sy, int32_t z, int16_t a, int16_t picnum,
                             int8_t dashade, char dapalnum, int32_t dastat, uint8_t daalpha, uint8_t dablend, int32_t cx1, int32_t cy1, int32_t cx2, int32_t cy2, int32_t uniqid);
void polymost_fillpolygon(int32_t npoints);
void polymost_initosdfuncs(void);
void polymost_drawrooms(void);
void polymost_prepareMirror(int32_t dax, int32_t day, int32_t daz, fix16_t daang, fix16_t dahoriz, int16_t mirrorWall);
void polymost_completeMirror();

int32_t polymost_maskWallHasTranslucency(uwalltype const * const wall);
int32_t polymost_spriteHasTranslucency(tspritetype const * const tspr);
int32_t polymost_spriteIsModelOrVoxel(tspritetype const * const tspr);

void polymost_resetVertexPointers(void);
void polymost_disableProgram(void);
void polymost_resetProgram(void);
void polymost_setTexturePosSize(vec4f_t const &texturePosSize);
void polymost_setHalfTexelSize(vec2f_t const &halfTexelSize);
char polymost_getClamp();
void polymost_setClamp(char clamp);
void polymost_setVisibility(float visibility);
void polymost_setFogEnabled(char fogEnabled);
void polymost_useColorOnly(char useColorOnly);
void polymost_usePaletteIndexing(char usePaletteIndexing);
void polymost_useDetailMapping(char useDetailMapping);
void polymost_useGlowMapping(char useGlowMapping);
void polymost_activeTexture(GLenum texture);
void polymost_bindTexture(GLenum target, uint32_t textureID);
void polymost_updatePalette(void);
void polymost_useShaderProgram(uint32_t shaderID);
GLuint polymost2_compileShader(GLenum shaderType, const char* const source, int* pLength = nullptr);

float* multiplyMatrix4f(float m0[4*4], const float m1[4*4]);

void polymost_glinit(void);
void polymost_glreset(void);

void polymost_init(void);

enum {
    INVALIDATE_ALL,
    INVALIDATE_ART,
    INVALIDATE_ALL_NON_INDEXED,
    INVALIDATE_ART_NON_INDEXED
};

void gltexinvalidate(int32_t dapicnum, int32_t dapalnum, int32_t dameth);
void gltexinvalidatetype(int32_t type);
int32_t polymost_printtext256(int32_t xpos, int32_t ypos, int16_t col, int16_t backcol, const char *name, char fontsize);

extern float curpolygonoffset;

extern float shadescale;
extern int32_t shadescale_unbounded;
extern uint8_t alphahackarray[MAXTILES];

extern int32_t r_usenewshading;
extern int32_t r_usetileshades;
extern int32_t r_npotwallmode;
extern int32_t r_brightnesshack;
extern int32_t polymostcenterhoriz;

extern int16_t globalpicnum;

extern float fogfactor[MAXPALOOKUPS];

// Compare with polymer_eligible_for_artmap()
static FORCE_INLINE int32_t eligible_for_tileshades(int32_t const picnum, int32_t const pal)
{
    return !usehightile || !hicfindsubst(picnum, pal, hictinting[pal].f & HICTINT_ALWAYSUSEART);
}

static FORCE_INLINE int polymost_usetileshades(void)
{
    return r_useindexedcolortextures && r_usetileshades && !(globalflags & GLOBAL_NO_GL_TILESHADES);
}

static inline float getshadefactor(int32_t const shade, int32_t const pal)
{
    // 8-bit tiles, i.e. non-hightiles and non-models, don't get additional
    // glColor() shading with r_usetileshades!
    if (videoGetRenderMode() == REND_POLYMOST && polymost_usetileshades() && eligible_for_tileshades(globalpicnum, globalpal))
        return 1.f;

    float const fshade = fogfactor[pal] != 0.f ? (float)shade / fogfactor[pal] : 0.f;

    if (r_usenewshading == 4)
        return max(min(1.f - (fshade * shadescale / frealmaxshade), 1.f), 0.f);

    float const shadebound = (float)((shadescale_unbounded || shade>=numshades) ? numshades : numshades-1);
    float const scaled_shade = fshade*shadescale;
    float const clamped_shade = min(max(scaled_shade, 0.f), shadebound);

    return ((float)(numshades-clamped_shade))/(float)numshades;
}

#define POLYMOST_CHOOSE_FOG_PAL(fogpal, pal) \
    ((fogpal) ? (fogpal) : (pal))
static FORCE_INLINE int32_t get_floor_fogpal(usectorptr_t const sec)
{
    return POLYMOST_CHOOSE_FOG_PAL(sec->fogpal, sec->floorpal);
}
static FORCE_INLINE int32_t get_ceiling_fogpal(usectorptr_t const sec)
{
    return POLYMOST_CHOOSE_FOG_PAL(sec->fogpal, sec->ceilingpal);
}
static FORCE_INLINE int32_t fogshade(int32_t const shade, int32_t const pal)
{
    polytintflags_t const tintflags = hictinting[pal].f;
    return (globalflags & GLOBAL_NO_GL_FOGSHADE || tintflags & HICTINT_NOFOGSHADE) ? 0 : shade;
}

static FORCE_INLINE int check_nonpow2(int32_t const x)
{
    return (x > 1 && (x&(x-1)));
}

// Are we using the mode that uploads non-power-of-two wall textures like they
// render in classic?
static FORCE_INLINE int polymost_is_npotmode(void)
{
    // The glinfo.texnpot check is so we don't have to deal with that case in
    // gloadtile_art().
    return glinfo.texnpot &&
        // r_npotwallmode is NYI for hightiles. We require r_hightile off
        // because in calc_ypanning(), the repeat would be multiplied by a
        // factor even if no modified texture were loaded.
        !usehightile &&
#ifdef NEW_MAP_FORMAT
        g_loadedMapVersion < 10 &&
#endif
        r_npotwallmode == 1;
}

static inline float polymost_invsqrt_approximation(float x)
{
#ifdef B_LITTLE_ENDIAN
    float const haf = x * .5f;
    union { float f; uint32_t i; } n = { x };
    n.i = 0x5f375a86 - (n.i >> 1);
    return n.f * (1.5f - haf * (n.f * n.f));
#else
    // this is the comment
    return 1.f / Bsqrtf(x);
#endif
}

// Flags of the <dameth> argument of various functions
enum {
    DAMETH_NOMASK = 0,
    DAMETH_MASK = 1,
    DAMETH_TRANS1 = 2,
    DAMETH_TRANS2 = 3,

    DAMETH_MASKPROPS = 3,

    DAMETH_CLAMPED = 4,

    DAMETH_WALL = 32,  // signals a texture for a wall (for r_npotwallmode)

    DAMETH_INDEXED = 512,

    DAMETH_N64 = 1024,
    DAMETH_N64_INTENSIVITY = 2048,
    DAMETH_N64_SCALED = 2097152,

    // used internally by polymost_domost
    DAMETH_BACKFACECULL = -1,

    // used internally by uploadtexture
    DAMETH_NODOWNSIZE = 4096,
    DAMETH_HI = 8192,
    DAMETH_NOFIX = 16384,
    DAMETH_NOTEXCOMPRESS = 32768,
    DAMETH_HASALPHA = 65536,
    DAMETH_ONEBITALPHA = 131072,
    DAMETH_ARTIMMUNITY = 262144,

    DAMETH_HASFULLBRIGHT = 524288,
    DAMETH_NPOTWALL = 1048576,

    DAMETH_UPLOADTEXTURE_MASK =
        DAMETH_HI |
        DAMETH_NODOWNSIZE |
        DAMETH_NOFIX |
        DAMETH_NOTEXCOMPRESS |
        DAMETH_HASALPHA |
        DAMETH_ONEBITALPHA |
        DAMETH_ARTIMMUNITY |
        DAMETH_HASFULLBRIGHT |
        DAMETH_NPOTWALL,
};

#define DAMETH_NARROW_MASKPROPS(dameth) (((dameth)&(~DAMETH_TRANS1))|(((dameth)&DAMETH_TRANS1)>>1))
EDUKE32_STATIC_ASSERT(DAMETH_NARROW_MASKPROPS(DAMETH_MASKPROPS) == DAMETH_MASK);
EDUKE32_STATIC_ASSERT(DAMETH_NARROW_MASKPROPS(DAMETH_CLAMPED) == DAMETH_CLAMPED);

#define TO_DAMETH_NODOWNSIZE(hicr_flags) (((hicr_flags)&HICR_NODOWNSIZE)<<8)
EDUKE32_STATIC_ASSERT(TO_DAMETH_NODOWNSIZE(HICR_NODOWNSIZE) == DAMETH_NODOWNSIZE);
#define TO_DAMETH_NOTEXCOMPRESS(hicr_flags) (((hicr_flags)&HICR_NOTEXCOMPRESS)<<15)
EDUKE32_STATIC_ASSERT(TO_DAMETH_NOTEXCOMPRESS(HICR_NOTEXCOMPRESS) == DAMETH_NOTEXCOMPRESS);
#define TO_DAMETH_ARTIMMUNITY(hicr_flags) (((hicr_flags)&HICR_ARTIMMUNITY)<<13)
EDUKE32_STATIC_ASSERT(TO_DAMETH_ARTIMMUNITY(HICR_ARTIMMUNITY) == DAMETH_ARTIMMUNITY);

// Do we want a NPOT-y-as-classic texture for this <dameth> and <ysiz>?
static FORCE_INLINE int polymost_want_npotytex(int32_t dameth, int32_t ysiz)
{
    return videoGetRenderMode() != REND_POLYMER &&  // r_npotwallmode NYI in Polymer
        polymost_is_npotmode() && (dameth&DAMETH_WALL) && check_nonpow2(ysiz);
}

// pthtyp pth->flags bits
enum pthtyp_flags {
    PTH_CLAMPED = 1,
    PTH_HIGHTILE = 2,
    PTH_SKYBOX = 4,
    PTH_HASALPHA = 8,
    PTH_HASFULLBRIGHT = 16,
    PTH_NPOTWALL = DAMETH_WALL,  // r_npotwallmode=1 generated texture
    PTH_FORCEFILTER = 64,

    PTH_INVALIDATED = 128,

    PTH_NOTRANSFIX = 256, // fixtransparency() bypassed

    PTH_INDEXED = 512,
    PTH_ONEBITALPHA = 1024,

    PTH_N64 = 2048,
    PTH_N64_INTENSIVITY = 4096,
    PTH_N64_SCALED = 8192,
};

typedef struct pthtyp_t
{
    struct pthtyp_t *next;
    struct pthtyp_t *ofb; // fullbright pixels
    hicreplctyp     *hicr;

    uint32_t        glpic;
    vec2f_t         scale;
    vec2_t          siz;
    int16_t         picnum;

    uint16_t        flags; // see pthtyp_flags
    polytintflags_t effects;
    char            palnum;
    char            shade;
    char            skyface;
} pthtyp;

// DAMETH -> PTH conversions
#define TO_PTH_CLAMPED(dameth) (((dameth)&DAMETH_CLAMPED)>>2)
EDUKE32_STATIC_ASSERT(TO_PTH_CLAMPED(DAMETH_CLAMPED) == PTH_CLAMPED);
#define TO_PTH_NOTRANSFIX(dameth) ((((~(dameth))&DAMETH_MASK)<<8)&(((~(dameth))&DAMETH_TRANS1)<<7))
EDUKE32_STATIC_ASSERT(TO_PTH_NOTRANSFIX(DAMETH_NOMASK) == PTH_NOTRANSFIX);
EDUKE32_STATIC_ASSERT(TO_PTH_NOTRANSFIX(DAMETH_MASK) == 0);
EDUKE32_STATIC_ASSERT(TO_PTH_NOTRANSFIX(DAMETH_TRANS1) == 0);
EDUKE32_STATIC_ASSERT(TO_PTH_NOTRANSFIX(DAMETH_MASKPROPS) == 0);
#define TO_PTH_INDEXED(dameth) ((dameth)&DAMETH_INDEXED)
EDUKE32_STATIC_ASSERT(TO_PTH_INDEXED(DAMETH_INDEXED) == PTH_INDEXED);
#define TO_PTH_N64(dameth) (((dameth)&DAMETH_N64)<<1)
EDUKE32_STATIC_ASSERT(TO_PTH_N64(DAMETH_N64) == PTH_N64);
#define TO_PTH_N64_INTENSIVITY(dameth) (((dameth)&DAMETH_N64_INTENSIVITY)<<1)
EDUKE32_STATIC_ASSERT(TO_PTH_N64_INTENSIVITY(DAMETH_N64_INTENSIVITY) == PTH_N64_INTENSIVITY);
#define TO_PTH_N64_SCALED(dameth) (((dameth)&DAMETH_N64_SCALED)>>8)
EDUKE32_STATIC_ASSERT(TO_PTH_N64_SCALED(DAMETH_N64_SCALED) == PTH_N64_SCALED);

extern void gloadtile_art(int32_t,int32_t,int32_t,int32_t,int32_t,pthtyp *,int32_t);
extern int32_t gloadtile_hi(int32_t,int32_t,int32_t,hicreplctyp *,int32_t,pthtyp *,int32_t,polytintflags_t);

extern int32_t globalnoeffect;
extern int32_t drawingskybox;
extern int32_t hicprecaching;
extern float fcosglobalang, fsinglobalang;
extern float fxdim, fydim, fydimen, fviewingrange;

extern char ptempbuf[MAXWALLSB<<1];

extern hitdata_t polymost_hitdata;

#include "texcache.h"

extern void polymost_setupglowtexture(int32_t texunits, int32_t tex);
extern void polymost_setupdetailtexture(int32_t texunits, int32_t tex);

#ifdef __cplusplus
}
#endif

#endif

#endif
