// 20170766 이건우

// 알림
// Mac에서 작업하여 헤더파일이 다를 수 있습니다.

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLUT/GLUT.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>

#define VAL 255
 
void set_texture(void);
 
typedef struct {unsigned char r, g, b;} rgb_t;
rgb_t **tex = 0;
int gwin;
GLuint texture;
int width, height;
int tex_w, tex_h;
double scale = 1./256;
double cx = -.6, cy = 0;
int max_iter = 128;
 
void render(void)
{
    double x = (double)width /tex_w, y = (double)height/tex_h;
 
    glClear(GL_COLOR_BUFFER_BIT);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
 
    glBindTexture(GL_TEXTURE_2D, texture);
 
    glBegin(GL_QUADS);
 
    glTexCoord2f(0, 0); glVertex2i(0, 0);
    glTexCoord2f(x, 0); glVertex2i(width, 0);
    glTexCoord2f(x, y); glVertex2i(width, height);
    glTexCoord2f(0, y); glVertex2i(0, height);
 
    glEnd();
    
    glFlush();
    glFinish();
}
 
int dump = 1;
 
void keypress(unsigned char key, int x, int y)
{
    switch(key) {
    // 종료
    case 'q':
            glFinish();
            glutDestroyWindow(gwin);
            return;
    // 원래대로
    case 27:
            scale = 1./256;
            cx = -.6; cy = 0;
            break;
    }
    set_texture();
}
 
void calc_mandel(void)
{
    int i, j, iter, min, max;
    rgb_t *px;
    double x, y, zx, zy, zx2, zy2;
    min = max_iter; max = 0;
    for (i = 0; i < height; i++) {
        px = tex[i];
        y = (i - height/2) * scale + cy;
        for (j = 0; j  < width; j++, px++) {
            x = (j - width/2) * scale + cx;
            iter = 0;
 
            zx = hypot(x - .25, y);
            if (x < zx - 2 * zx * zx + .25) iter = max_iter;
            if ((x + 1)*(x + 1) + y * y < 1/16) iter = max_iter;
 
            zx = zy = zx2 = zy2 = 0;
            for (; iter < max_iter && zx2 + zy2 < 4; iter++) {
                zy = 2 * zx * zy + y;
                zx = zx2 - zy2 + x;
                zx2 = zx * zx;
                zy2 = zy * zy;
            }
            if (iter < min) min = iter;
            if (iter > max) max = iter;
            *(unsigned short *)px = iter;
        }
    }
 
    for (i = 0; i < height; i++)
        for (j = 0, px = tex[i]; j  < width; j++, px++)
            px->r = px->g = px->b = 255 * (max - *(unsigned short*)px) / (max - min);
}
 
void alloc_tex(void)
{
    int i, ow = tex_w, oh = tex_h;
 
    for (tex_w = 1; tex_w < width;  tex_w <<= 1);
    for (tex_h = 1; tex_h < height; tex_h <<= 1);
 
    if (tex_h != oh || tex_w != ow)
        tex = realloc(tex, tex_h * tex_w * 3 + tex_h * sizeof(rgb_t*));
 
    for (tex[0] = (rgb_t *)(tex + tex_h), i = 1; i < tex_h; i++)
        tex[i] = tex[i - 1] + tex_w;
}
 
void set_texture(void)
{
    alloc_tex();
    calc_mandel();
 
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, tex_w, tex_h,
        0, GL_RGB, GL_UNSIGNED_BYTE, tex[0]);
 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    render();
}
 
void mouseclick(int button, int state, int x, int y)
{
    if (state != GLUT_UP) return;
 
    cx += (x - width / 2) * scale;
    cy -= (y - height/ 2) * scale;
 
    switch(button) {
    case GLUT_LEFT_BUTTON: /* zoom in */
        if (scale > abs(x) * 1e-16 && scale > abs(y) * 1e-16)
            scale /= 2;
        break;
    case GLUT_RIGHT_BUTTON:
        scale *= 2;
        break;
    }
    set_texture();
}
 
 
void resize(int w, int h)
{
    printf("resize %d %d\n", w, h);
    width = w;
    height = h;
 
    glViewport(0, 0, w, h);
    glOrtho(0, w, 0, h, -1, 1);
 
    set_texture();
}
 
void init_gfx(int *c, char **v)
{
    glutInit(c, v);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(640, 480);
    
    gwin = glutCreateWindow("Mandelbrot");
    glutDisplayFunc(render);
 
    glutKeyboardFunc(keypress);
    glutMouseFunc(mouseclick);
    glutReshapeFunc(resize);
    glGenTextures(1, &texture);
    set_texture();
}
 
int main(int c, char **v)
{
    init_gfx(&c, v);
    
    glutMainLoop();
    return 0;
}
