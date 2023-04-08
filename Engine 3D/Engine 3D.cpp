#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <iostream>

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    vec3d p[3];
};

struct mesh
{
    std::vector<triangle> tris;
};

struct mat4x4
{
    float m[4][4] = { 0 };
};
class olcEngine3D : public olc::PixelGameEngine
{

public:
    olcEngine3D()
    {
        sAppName = "3D Demo";
    }

private:
    mesh meshCube;
    mat4x4 mat;

    vec3d virtualCamera;



    float fTheta;

    void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m)
    {
		o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
		o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
		o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
		float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f)
        {
            o.x /= w;
            o.y /= w;
            o.z /= w;
        }
    }

    float magnitude(vec3d findmag) {
        float mag = sqrtf(findmag.x * findmag.x + findmag.y * findmag.y + findmag.z * findmag.z);
        return mag;
    }

    void normalize(vec3d normalizedvector) {
        float mag = magnitude(normalizedvector);
        normalizedvector.x /= mag;
        normalizedvector.y /= mag;
        normalizedvector.z /= mag;
    }

public:
    bool OnUserCreate() override
    {
        // Create each triangle in the mesh
        meshCube.tris = {
            // SOUTH
            { 0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f },

            // EAST
            { 1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f },

            // NORTH
            { 1.0f, 0.0f, 1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f },

            // WEST
            { 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f },

            // TOP
            { 0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f },

            // BOTTOM
            { 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f },

        };

        // Projection Matrix
        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth();
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        mat.m[0][0] = fAspectRatio * fFovRad;
        mat.m[1][1] = fFovRad;
        mat.m[2][2] = fFar / (fFar - fNear);
        mat.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        mat.m[2][3] = 1.0f;
        mat.m[3][3] = 0.0f;


        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {

        FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::RED);

        mat4x4 matRotZ, matRotX;
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z Matrix
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X Matrix
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        // Loop through all triangles in the mesh
        for (auto tri : meshCube.tris)
        {
            triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate with Z rotation matrix
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);
            
            // Combine Z rotation matrix output with X rotation matrix
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            // Move and offset on Z axis
            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;


            // Calculate normal
            vec3d normal, line1, line2;

            line1.x = triTranslated.p[1].x - triTranslated.p[0].x;
            line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            line2.x = triTranslated.p[2].x - triTranslated.p[0].x;
            line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            normalize(normal);


            // Draw only if normal is negative in comparison with camera
            if (normal.x * (triTranslated.p[0].x - virtualCamera.x) +
                normal.y * (triTranslated.p[0].y - virtualCamera.y) +
                normal.z * (triTranslated.p[0].z - virtualCamera.z) < 0.0f)
            {
                // Illumination
                vec3d light_direction = { 0.0f, 0.0f, -1.0f };

                // Create triangle in 2D
                MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], mat);
                MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], mat);
                MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], mat);

                // Scale triangle
                triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f;
                triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;

                triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
                triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
                triProjected.p[2].y *= 0.5f * (float)ScreenHeight();

                // Draw the triangle
                DrawTriangle(triProjected.p[0].x, triProjected.p[0].y, triProjected.p[1].x, triProjected.p[1].y, triProjected.p[2].x, triProjected.p[2].y, olc::WHITE);
            }
        }
        return true;
    }
};

int main()
{
    olcEngine3D demo;
    if (demo.Construct(256, 240, 4, 4))
    {
        demo.Start();
    }
    else
    {
        std::cout << "error";
    }

}
