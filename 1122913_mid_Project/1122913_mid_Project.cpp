#include <stdlib.h>
#include <GL/glut.h>
#include <iostream>
#include <math.h>
#include <GL/glu.h>
#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

// 引入 stb_image.h
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// 手動定義 GL_CLAMP_TO_EDGE
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif

// 全局變數
int _width = 500;
int _height = 500;
int _ArmRotation1 = -40;
int _ArmRotation2 = 90;
int _multiBucketRotation = -90;
double _m_angleY;
double _m_angleX;
int _mouseLastPosionX;
int _mouseLastPosionY;
double _rotationValueX = 1.0;
double _excavatorMovement = 0;
double _wheelRotation = 0;
double _cabinRotation = 0;
bool drag = false;
static const GLfloat warmYellow[] = { 1.0, 0.84, 0.0, 1.0 };
static const GLfloat yellow[] = { 0.9, 0.7, 0.1, 1.0 };
static const GLfloat blue[] = { 0.0, 0.0, 1.0, 1.0 };
static const GLfloat red[] = { 1.0, 0.0, 0.0, 1.0 };
static const GLfloat gray[] = { 0.5, 0.5, 0.5, 1.0 };
static const GLfloat black[] = { 0.0, 0.0, 0.0, 1.0 };
static const GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
static const GLfloat darkGray[] = { 0.2, 0.2, 0.2, 1.0 };

// 儲存 "TRUCK" 文字的紋理 ID
GLuint truckTexture;

// 設置材質
void setMaterial(const GLfloat* materialDiffuse, const GLfloat* materialAmbient) {
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
    GLfloat material_specular[] = { 0.3, 0.3, 0.3, 1.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0);
}

// 繪製輪胎上的凹槽
void drawWheelGroove() {
    setMaterial(black, black);
    for (int i = 0; i < 12; ++i) {
        glPushMatrix();
        glRotated(i * 30.0, 0, 0, 1);
        glTranslated(0.0, 0.0, 0.0);
        glPushMatrix();
        glTranslated(1.5, 0.0, 0.0);
        glScaled(0.3, 1.4, 0.5);
        glutSolidCube(1);
        glPopMatrix();
        glPopMatrix();
    }

    setMaterial(red, black);
    for (int i = 0; i < 12; ++i) {
        glPushMatrix();
        glRotated(i * 30.0 + 15.0, 0, 0, 1);
        glTranslated(1.4, 0.0, 0.0);
        glScaled(0.2, 0.2, 0.2);
        glutSolidCube(1);
        glPopMatrix();
    }
}

// 繪製輪胎
void drawWheel(double zPosition) {
    glPushMatrix();
    setMaterial(black, darkGray);
    glRotated(_wheelRotation, 0, 0, 1);
    glScaled(0.8, 0.8, 1.5);
    glutSolidTorus(0.5, 1, 20, 30);

    setMaterial(gray, gray);
    glPushMatrix();
    glScaled(0.3, 0.3, 0.2);
    glutSolidTorus(0.1, 0.5, 15, 20);
    glPopMatrix();

    setMaterial(gray, gray);
    int numSpokes = 10;
    float spokeLength = 0.7;
    float spokeWidth = 0.1;
    float spokeThickness = 0.05;

    for (int i = 0; i < numSpokes; i++) {
        glPushMatrix();
        float angle = i * (360.0 / numSpokes);
        glRotated(angle, 0, 0, 1);
        glTranslated(spokeLength / 2, 0, 0);
        glScaled(spokeLength, spokeWidth, spokeThickness);
        glutSolidCube(1);
        glPopMatrix();
    }

    setMaterial(white, gray);
    glPushMatrix();
    glTranslated(0.0, 0.0, -0.45);
    glScaled(0.8, 0.8, 0.05);
    glutSolidTorus(0.1, 1.0, 10, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.0, 0.0, 0.45);
    glScaled(0.8, 0.8, 0.05);
    glutSolidTorus(0.1, 1.0, 10, 20);
    glPopMatrix();

    drawWheelGroove();
    glPopMatrix();
}

// 繪製所有輪胎
void drawWheels() {
    glPushMatrix();
    glTranslated(0.25, -3.0, 0.9);
    drawWheel(0.9);
    glTranslated(3.5, 0, 0);
    drawWheel(0.9);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.25, -3.0, -3.5);
    drawWheel(-3.5);
    glTranslated(3.5, 0, 0);
    drawWheel(-3.5);
    glPopMatrix();
}

// 繪製底座
void drawBase() {
    glPushMatrix();
    setMaterial(gray, gray);
    glTranslated(2, -2.5, -1.3);
    glPushMatrix();
    glScaled(3.5, 0.7, 3.0);
    glutSolidCube(1);
    glPopMatrix();
    glPopMatrix();
}

// 繪製艙體底部的圓環
void drawTorus() {
    glPushMatrix();
    setMaterial(gray, gray);
    glTranslated(2.0, -1.75, -1.5);
    glScaled(0.7, 0.7, 0.7);
    glRotated(90, 1, 0, 0);
    glutSolidTorus(0.5, 1, 4, 8);
    glPopMatrix();
}

// 繪製艙體
void drawCabin() {
    glPushMatrix();
    setMaterial(warmYellow, warmYellow);
    glTranslated(2.2, -1.0, -2.1);
    glPushMatrix();
    glScaled(3.5, 1.0, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    // 在艙的右側面添加黑色垂直線條（通風口）
    setMaterial(black, black);
    glPushMatrix();
    glTranslated(0, 0.95, 2.35 - 0.1);
    int numLines = 5;
    float lineSpacing = 0.5 / (numLines - 3);
    float lineHeight = 0.40;
    float lineWidth = 0.1;
    float lineDepth = 0.07;

    for (int i = 0; i < numLines; i++) {
        glPushMatrix();
        float xPos = 0.5 + i * lineSpacing;
        glTranslated(xPos, 0, 0);
        glScaled(lineWidth, lineHeight, lineDepth);
        glutSolidCube(1);
        glPopMatrix();
    }
    glPopMatrix();

    // 在艙的左側面添加黑色垂直線條（通風口）
    setMaterial(black, black);
    glPushMatrix();
    glTranslated(0, 0.95, -0.85 + 0.1); // 調整 Z 坐標到左側面
    for (int i = 0; i < numLines; i++) {
        glPushMatrix();
        float xPos = 0.5 + i * lineSpacing;
        glTranslated(xPos, 0, 0);
        glScaled(lineWidth, lineHeight, lineDepth);
        glutSolidCube(1);
        glPopMatrix();
    }
    glPopMatrix();

    // 添加右側面 logo（使用 truck_text.png，包含紅色三角形和 "TRUCK" 文字）
    glPushMatrix();
    glTranslated(0, 0.95, 2.35 - 0.05);
    glTranslated(1.0, -0.5, 0);

    // 使用紋理繪製 "TRUCK" 文字和三角形
    glPushMatrix();
    glTranslated(-1.4, -0.5, 0.01); // 調整位置

    // 啟用紋理並綁定 "TRUCK" 紋理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, truckTexture);

    // 啟用 Alpha 混合以支援透明背景
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 關閉光照，確保紋理顏色不受光照影響
    glDisable(GL_LIGHTING);

    // 繪製一個平面來貼上 "TRUCK" 紋理和三角形，放大範圍
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 1.0); glVertex3f(-0.7, -0.35, 0.0); // 左下
    glTexCoord2f(1.0, 1.0); glVertex3f(0.7, -0.35, 0.0);  // 右下
    glTexCoord2f(1.0, 0.0); glVertex3f(0.7, 0.35, 0.0);   // 右上
    glTexCoord2f(0.0, 0.0); glVertex3f(-0.7, 0.35, 0.0);  // 左上
    glEnd();

    // 恢復光照和紋理狀態
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();
    glPopMatrix();

    // 添加左側面 logo（使用 truck_text.png）
    glPushMatrix();
    glTranslated(0, 0.95, -0.85 + 0.05); // 調整 Z 坐標到左側面
    glTranslated(1.0, -0.5, 0);

    // 使用紋理繪製 "TRUCK" 文字和三角形
    glPushMatrix();
    glTranslated(-1.4, -0.5, -0.01); // 調整位置，稍微向內偏移

    // 啟用紋理並綁定 "TRUCK" 紋理
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, truckTexture);

    // 啟用 Alpha 混合以支援透明背景
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 關閉光照，確保紋理顏色不受光照影響
    glDisable(GL_LIGHTING);

    // 繪製一個平面來貼上 "TRUCK" 紋理和三角形，放大範圍
    // 左側面法向量相反，需要逆序繪製頂點，並調整紋理坐標以避免鏡像
    glBegin(GL_QUADS);
    glTexCoord2f(1.0, 1.0); glVertex3f(-0.7, -0.35, 0.0); // 左下（右側的右下）
    glTexCoord2f(0.0, 1.0); glVertex3f(0.7, -0.35, 0.0);  // 右下（右側的左下）
    glTexCoord2f(0.0, 0.0); glVertex3f(0.7, 0.35, 0.0);   // 右上（右側的左上）
    glTexCoord2f(1.0, 0.0); glVertex3f(-0.7, 0.35, 0.0);  // 左上（右側的右上）
    glEnd();

    // 恢復光照和紋理狀態
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
    glBindTexture(GL_TEXTURE_2D, 0);

    glPopMatrix();
    glPopMatrix();

    // 以下是艙體的其他部分
    setMaterial(warmYellow, warmYellow);
    glPushMatrix();
    glTranslated(0, 0, 1.5);
    glScaled(3.5, 1.0, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    setMaterial(black, black);
    glPushMatrix();
    glTranslated(-0.875, 1.50, 1.5);
    glScaled(1.75, 2.0, 1.5);

    glPushMatrix();
    glTranslated(-0.45, -0.0875, 0.45);
    glScaled(0.1, 0.775, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.45, -0.0875, 0.45);
    glScaled(0.1, 0.775, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.45, -0.0875, -0.45);
    glScaled(0.1, 0.775, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0.45, -0.0875, -0.45);
    glScaled(0.1, 0.775, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, 0.4, 0);
    glScaled(1.0, 0.2, 1.0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslated(0, -0.4, 0);
    glScaled(1.0, 0.2, 1.0);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();

    setMaterial(warmYellow, warmYellow);
    glPushMatrix();
    glTranslated(0.875, 0.9, 1.5);
    glScaled(1.75, 0.8, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    setMaterial(warmYellow, warmYellow);
    glTranslated(0, 0.9, 0.0);
    glPushMatrix();
    glScaled(3.5, 0.8, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    setMaterial(black, black);
    glTranslated(1.2, 0.45, 0.7);
    glPushMatrix();
    glScaled(1.5, 0.3, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    setMaterial(black, black);
    glTranslated(0.6, -0.45, 0.0);
    glRotatef(90, 0.0, 0.0, 1.0);
    glPushMatrix();
    glScaled(1.0, 0.3, 1.5);
    glutSolidCube(1);
    glPopMatrix();

    glPopMatrix();
}

// 繪製手臂部分
void drawArmPart(bool isUpperArm, bool showJointSphere) {
    glPushMatrix();
    setMaterial(warmYellow, warmYellow);

    float length = isUpperArm ? 3.5 : 5.0;
    float width = 1.0;
    float bottomHeight, topHeight;

    if (isUpperArm) {
        bottomHeight = 0.55;
        topHeight = 0.8;
    }
    else {
        bottomHeight = 0.55;
        topHeight = 0.8;
    }

    float v0[] = { -length / 2, -bottomHeight / 2, -width / 2 };
    float v1[] = { -length / 2, -bottomHeight / 2,  width / 2 };
    float v4[] = { -length / 2,  bottomHeight / 2, -width / 2 };
    float v5[] = { -length / 2,  bottomHeight / 2,  width / 2 };
    float v2[] = { length / 2, -topHeight / 2,  width / 2 };
    float v3[] = { length / 2, -topHeight / 2, -width / 2 };
    float v6[] = { length / 2,  topHeight / 2,  width / 2 };
    float v7[] = { length / 2,  topHeight / 2, -width / 2 };

    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0); glVertex3fv(v0); glVertex3fv(v1); glVertex3fv(v2); glVertex3fv(v3);
    glNormal3f(0, 1, 0);  glVertex3fv(v4); glVertex3fv(v5); glVertex3fv(v6); glVertex3fv(v7);
    glNormal3f(0, 0, -1); glVertex3fv(v0); glVertex3fv(v3); glVertex3fv(v7); glVertex3fv(v4);
    glNormal3f(0, 0, 1);  glVertex3fv(v1); glVertex3fv(v2); glVertex3fv(v6); glVertex3fv(v5);
    glNormal3f(-1, 0, 0); glVertex3fv(v0); glVertex3fv(v1); glVertex3fv(v5); glVertex3fv(v4);
    glNormal3f(1, 0, 0);  glVertex3fv(v2); glVertex3fv(v3); glVertex3fv(v7); glVertex3fv(v6);
    glEnd();

    if (showJointSphere) {
        glPushMatrix();
        glTranslated(length / 2, 0, 0);
        glutSolidSphere(0.6, 10, 10);
        glPopMatrix();
    }

    glPopMatrix();
}

// 繪製挖斗
void drawMultipartBucket() {
    glPushMatrix();
    static const GLfloat orange[] = { 1.0, 0.6, 0.0, 1.0 };
    setMaterial(orange, orange);

    glPushMatrix();
    glScaled(2.0, 1.5, 2.5);

    float back_height = 0.5;
    float back_width = 0.5;
    float bottom_front_y = -0.3;
    float bottom_width = 0.5;
    float front_height = -0.1;
    float front_width = 0.5;
    float length = 0.4;

    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glVertex3f(-length, -back_height, -back_width);
    glVertex3f(-length, -back_height, back_width);
    glVertex3f(-length, back_height, back_width);
    glVertex3f(-length, back_height, -back_width);

    glNormal3f(0, -1, 0);
    glVertex3f(-length, -back_height, -bottom_width);
    glVertex3f(-length, -back_height, bottom_width);
    glVertex3f(length, bottom_front_y, bottom_width);
    glVertex3f(length, bottom_front_y, -bottom_width);

    glNormal3f(0, 0, -1);
    glVertex3f(-length, -back_height, -back_width);
    glVertex3f(-length, back_height, -back_width);
    glVertex3f(length, front_height, -front_width);
    glVertex3f(length, bottom_front_y, -bottom_width);

    glNormal3f(0, 0, 1);
    glVertex3f(-length, -back_height, back_width);
    glVertex3f(-length, back_height, back_width);
    glVertex3f(length, front_height, front_width);
    glVertex3f(length, bottom_front_y, bottom_width);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glScaled(2.0, 1.5, 2.5);
    glBegin(GL_TRIANGLES);
    setMaterial(orange, orange);
    float tooth_height = 0.2;
    float tooth_width = 0.1;
    int num_teeth = 5;
    float total_teeth_width = tooth_width * 2 * num_teeth;
    float available_width = front_width * 2 - tooth_width * 2;
    float tooth_spacing = available_width / (num_teeth - 1);
    float start_z = -front_width + tooth_width;

    for (int i = 0; i < num_teeth; i++) {
        float z = start_z + i * tooth_spacing;
        glNormal3f(1, 0, 0);
        glVertex3f(length, bottom_front_y, z - tooth_width);
        glVertex3f(length, bottom_front_y, z + tooth_width);
        glVertex3f(length + tooth_height, bottom_front_y + 0.2, z);
    }
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.8, 0.7, -0.6);
    setMaterial(orange, orange);
    glScaled(0.2, 0.4, 0.1);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);

    glNormal3f(-1, 0, 0);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 0, -1);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);

    glNormal3f(0, 0, 1);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 1, 0);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);

    glNormal3f(0, -1, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.8, 0.7, 0.6);
    setMaterial(orange, orange);
    glScaled(0.2, 0.4, 0.1);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);

    glNormal3f(-1, 0, 0);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 0, -1);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);

    glNormal3f(0, 0, 1);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 1, 0);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);

    glNormal3f(0, -1, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslated(-0.8, 0.4, 0);
    setMaterial(orange, orange);
    glScaled(0.1, 0.2, 1.8);
    glBegin(GL_QUADS);
    glNormal3f(1, 0, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);

    glNormal3f(-1, 0, 0);
    glVertex3f(-0.5, -0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 0, -1);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(-0.5, 0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);

    glNormal3f(0, 0, 1);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(0.5, -0.5, 0.5);

    glNormal3f(0, 1, 0);
    glVertex3f(-0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, -0.5);
    glVertex3f(0.5, 0.5, 0.5);
    glVertex3f(-0.5, 0.5, 0.5);

    glNormal3f(0, -1, 0);
    glVertex3f(-0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, -0.5);
    glVertex3f(0.5, -0.5, 0.5);
    glVertex3f(-0.5, -0.5, 0.5);
    glEnd();
    glPopMatrix();

    glPopMatrix();
}

// 繪製挖掘機手臂
void drawExcavatorArm() {
    glPushMatrix();
    glTranslated(-1.35, -0.1, -2);
    glTranslated(1.75, 0, 0);
    glRotated(_ArmRotation1, 0, 0, 1);
    glTranslated(-1.75, 0, 0);
    drawArmPart(true, true);

    glPushMatrix();
    glTranslated(-3.5, 0.3, 0);
    glTranslated(1.75, 0, 0);
    glRotated(_ArmRotation2, 0, 0, 1);
    glTranslated(-1.75, 0, 0);
    drawArmPart(false, false);

    glPushMatrix();
    glTranslated(-2.55, 0, 0);
    glRotated(_multiBucketRotation, 0, 0, 1);
    glTranslated(0.8, -0.7, 0);
    drawMultipartBucket();
    glPopMatrix();

    glPopMatrix();
    glPopMatrix();
}

// 鍵盤事件處理
void KeyboardEvents(unsigned char key, int x, int y) {
    switch (key) {
    case 'r': if (_cabinRotation > -180) _cabinRotation -= 5; break;
    case 't': if (_cabinRotation < 180) _cabinRotation += 5; break;
    case 'w': if (_ArmRotation2 > 0) _ArmRotation2 -= 4; break;
    case 's': if (_ArmRotation2 < 95) _ArmRotation2 += 4; break;
    case 'a': if (_ArmRotation1 > -100) _ArmRotation1 -= 4; break;
    case 'd': if (_ArmRotation1 < -40) _ArmRotation1 += 4; break;
    default: return;
    }
    glutPostRedisplay();
}

// 方向鍵事件處理
void ArrowEvents(int key, int x, int y) {
    switch (key) {
    case GLUT_KEY_LEFT:
        _excavatorMovement -= 0.1;
        _wheelRotation += 8;
        break;
    case GLUT_KEY_RIGHT:
        _excavatorMovement += 0.1;
        _wheelRotation -= 8;
        break;
    case GLUT_KEY_UP:
        if (_multiBucketRotation < -60) _multiBucketRotation += 4; // 挖斗向上旋轉
        break;
    case GLUT_KEY_DOWN:
        if (_multiBucketRotation > -225) _multiBucketRotation -= 4; // 挖斗向下旋轉
        break;
    }
    glutPostRedisplay();
}

// 顯示函數
void mydisplay() {
    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(10.0, 3.0, 15.0, 2.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    //gluLookAt(7.0, 3.0, 15.0, 2.0, 0.0, 0.0, 0.0, 1.0, 0.0);


    glPushMatrix();
    glTranslated(_excavatorMovement, -1, 0);
    drawWheels();
    drawBase();
    glPushMatrix();
    glTranslated(1.75, 0, -1.5);
    glRotated(_cabinRotation, 0, 1, 0);
    glTranslated(-1.75, 0, 1.5);
    drawTorus();
    drawCabin();
    drawExcavatorArm();
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}

// 初始化函數
void init() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-9.5, 9.5, -9.5, 9.5, -150.0, 150.0);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    // 檢查 OpenGL 版本
    const GLubyte* version = glGetString(GL_VERSION);
    if (!version) {
        std::cerr << "無法獲取 OpenGL 版本，可能是上下文未正確初始化" << std::endl;
        std::cin.get();
        exit(1);
    }
    std::cout << "OpenGL 版本: " << version << std::endl;

    // 使用 stb_image.h 載入高解析度圖片
    int width, height, nrChannels;
    unsigned char* data = stbi_load("truck_text.png", &width, &height, &nrChannels, 0);
    if (!data) {
        std::cerr << "無法載入 truck_text_highres.png: " << stbi_failure_reason() << std::endl;
        std::cin.get();
        exit(1);
    }

    std::cout << "圖片寬度: " << width << ", 高度: " << height << ", 通道數: " << nrChannels << std::endl;
    if (nrChannels != 3 && nrChannels != 4) {
        std::cerr << "不支援的圖片通道數: " << nrChannels << std::endl;
        std::cin.get();
        exit(1);
    }

    // 建立 OpenGL 紋理
    glGenTextures(1, &truckTexture);
    glBindTexture(GL_TEXTURE_2D, truckTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (nrChannels == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    glEnable(GL_TEXTURE_2D);

    // 設置光源
    GLfloat light_position[] = { 5.0, 5.0, 5.0, 1.0 };
    GLfloat light_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat light_specular[] = { 0.3, 0.3, 0.3, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // 啟用抗鋸齒與透明混合
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}


// 主函數
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
    glutInitWindowSize(800, 700);
    glutCreateWindow("excavator");

    // 檢查 OpenGL 錯誤
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        std::cerr << "OpenGL 初始化錯誤: " << gluErrorString(err) << std::endl;
        std::cerr << "按任意鍵繼續..." << std::endl;
        std::cin.get();
        return 1;
    }

    glEnable(GL_MULTISAMPLE);

    init();
    glutDisplayFunc(mydisplay);
    glutKeyboardFunc(KeyboardEvents);
    glutSpecialFunc(ArrowEvents);
    glutMainLoop();
    return 0;
}