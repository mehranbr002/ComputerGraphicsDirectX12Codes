#include "d3dUtility.h"
#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

const int Width = 1080;
const int Height = 720;
IDirect3DVertexBuffer9* VB = 0;


IDirect3DDevice9* Device = 0;

ID3DXMesh* Objects[15] = { 0 };
D3DXMATRIX  Worlds[15];
D3DXMATRIX ObjWorldMatrices[15];
D3DMATERIAL9 Materials[15];
D3DMATERIAL9 Mtrls[15];


IDirect3DTexture9* FloorTex = 0;
IDirect3DTexture9* WallTex = 0;
IDirect3DTexture9* MirrorTex = 0;

D3DMATERIAL9 FloorMtrl = d3d::RED_MTRL;
D3DMATERIAL9 WallMtrl = d3d::WHITE_MTRL;
D3DMATERIAL9 MirrorMtrl = d3d::WHITE_MTRL;

// images
IDirect3DTexture9* image_wall_tex = 0;
IDirect3DTexture9* wall_tex = 0;

// mirror
D3DMATERIAL9 mirror_material = d3d::WHITE_MTRL;
IDirect3DTexture9* mirror_texture = 0;


struct Vertex
{
	Vertex() {}
	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float u, float v)
	{
		_x = x;  _y = y;  _z = z;
		_nx = nx; _ny = ny; _nz = nz;
		_u = u;  _v = v;
	}
	float _x, _y, _z;
	float _nx, _ny, _nz;
	float _u, _v;

	static const DWORD FVF;
};
const DWORD Vertex::FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;


// last move for w and s
static string last_move = "";
static boolean hands_positivity = true;
// degree of hands and legs
static float walk_degree_forward = 0.0f; // y

// positions of walking movements
static float walk_position_sides = 0.0f; // x
static float walk_position_Yz = 0.0f; // y
static float walk_position_forward = 0.0f; // z

// limits of plane with x, y, z
static float plane_x = 20.0f;
static float plane_y = -7.4f;
static float plane_z = 20.0f;

// speed of walking
static float speed = 0.01f;



// function to render the science of the floor and wall
void RenderScene() {

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);

	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);

	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// draw the floor
	Device->SetMaterial(&d3d::BLUE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	// draw the wall
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 2);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 18, 2);
/*
	// draw the wall
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, image_wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 12, 2);

	// draw the roof
	Device->SetMaterial(&d3d::WHITE_MTRL);
	Device->SetTexture(0, wall_tex);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 24, 2);

	// draw the mirror
	Device->SetMaterial(&mirror_material);
	Device->SetTexture(0, mirror_texture);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);*/
}
/*
// function to render the mirror
void RenderMirror() {
	Device->SetRenderState(D3DRS_STENCILENABLE, true);
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	Device->SetRenderState(D3DRS_STENCILREF, 0x1);
	Device->SetRenderState(D3DRS_STENCILMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILWRITEMASK, 0xffffffff);
	Device->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_KEEP);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);

	// disable writes to the depth and back buffers
	Device->SetRenderState(D3DRS_ZWRITEENABLE, false);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// draw the mirror to the stencil buffer
	Device->SetStreamSource(0, VB, 0, sizeof(Vertex));
	Device->SetFVF(Vertex::FVF);
	Device->SetMaterial(&mirror_material);
	Device->SetTexture(0, mirror_texture);

	D3DXMATRIX I;
	D3DXMatrixIdentity(&I);
	Device->SetTransform(D3DTS_WORLD, &I);
	Device->DrawPrimitive(D3DPT_TRIANGLELIST, 30, 2);

	// re-enable depth writes
	Device->SetRenderState(D3DRS_ZWRITEENABLE, true);

	// only draw reflected to the pixels where the mirror was drawn to.
	Device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	Device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);

	// position reflection
	D3DXMATRIX W;
	D3DXMATRIX ref = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, -1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
	};

	D3DXMATRIX tra = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, plane_z , 1.0f
	};

	ref *= tra;

	// clear depth buffer and blend the reflected teapot with the mirror
	Device->Clear(0, 0, D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);

	// find the exact reflection in mirror
	W = Worlds[0] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[0]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[0]->DrawSubset(0);

	W = Worlds[1] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[1]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[1]->DrawSubset(0);

	W = Worlds[5] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[5]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[5]->DrawSubset(0);

	W = Worlds[6] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[6]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[6]->DrawSubset(0);

	W = Worlds[8] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[8]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[8]->DrawSubset(0);

	W = Worlds[9] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[9]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[9]->DrawSubset(0);

	W = Worlds[10] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[10]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[10]->DrawSubset(0);

	W = Worlds[11] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[11]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[11]->DrawSubset(0);

	W = Worlds[12] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[12]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[12]->DrawSubset(0);

	W = Worlds[13] * ref;
	Device->SetTransform(D3DTS_WORLD, &W);
	Device->SetMaterial(&Materials[13]);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	Objects[13]->DrawSubset(0);

	// Restore render states.
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	Device->SetRenderState(D3DRS_STENCILENABLE, false);
	Device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
*/
/*
// function to translate the matrix
D3DXMATRIX translate(D3DXMATRIX base, float x, float y, float z) {
	D3DXMATRIX translateMatrix(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		x, y, z, 1);

	D3DXMATRIX result = base * translateMatrix;
	return result;
}

// function to find the position of objects after rotating in given axes
D3DXMATRIX PositionFinder(float length, float lengthBefore, float elementPosition[], float degreeOrigin,
	float degreeFinal, string directionOrigin, string directionFinal) {

	D3DXMATRIX tra2Origin(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	D3DXMATRIX rotInOrigin;
	if (directionOrigin == "x")
		rotInOrigin = D3DXMATRIX(1, 0, 0, 0,
			0, cosf(degreeOrigin), sinf(degreeOrigin), 0,
			0, -sinf(degreeOrigin), cosf(degreeOrigin), 0,
			0, 0, 0, 1);

	else if (directionOrigin == "y")
		rotInOrigin = D3DXMATRIX(cosf(degreeOrigin), 0, -sinf(degreeOrigin), 0,
			0, 1, 0, 0,
			sinf(degreeOrigin), 0, cosf(degreeOrigin), 0,
			0, 0, 0, 1);

	else
		rotInOrigin = D3DXMATRIX(cosf(degreeOrigin), sinf(degreeOrigin), 0, 0,
			-sinf(degreeOrigin), cosf(degreeOrigin), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

	D3DXMATRIX tra2Length(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, -lengthBefore - length / 2, 0, 1);

	D3DXMATRIX rotFromLength;
	if (directionFinal == "x")
		rotFromLength = D3DXMATRIX(1, 0, 0, 0,
			0, cosf(degreeFinal), sinf(degreeFinal), 0,
			0, -sinf(degreeFinal), cosf(degreeFinal), 0,
			0, 0, 0, 1);

	else if (directionFinal == "y")
		rotFromLength = D3DXMATRIX(cosf(degreeFinal), 0, -sinf(degreeFinal), 0,
			0, 1, 0, 0,
			sinf(degreeFinal), 0, cosf(degreeFinal), 0,
			0, 0, 0, 1);

	else
		rotFromLength = D3DXMATRIX(cosf(degreeFinal), sinf(degreeFinal), 0, 0,
			-sinf(degreeFinal), cosf(degreeFinal), 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);

	D3DXMATRIX tra2Final(1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		elementPosition[0], elementPosition[1] + lengthBefore + length / 2, elementPosition[2], 1);

	D3DXMATRIX result = tra2Origin * rotInOrigin * tra2Length * rotFromLength * tra2Final;
	return result;
}

// function to reset positions of the body to 90 degrees
void resetBodyWall() {
	Worlds[9] = PositionFinder(3.0f, 0, new float[] {-2.85f - walk_position_sides, -2.5f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[10] = PositionFinder(3.0f, 0, new float[] {+2.85f - walk_position_sides, -2.5f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[11] = PositionFinder(3.0f, 0, new float[] {-1.2f - walk_position_sides, -5.8f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");

	Worlds[12] = PositionFinder(3.0f, 0, new float[] {1.2f - walk_position_sides, -5.8f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, 0, "x", "x");
}
// function to walk forward and backward in X
void walkLeftRight(string direction) {
	// prevent from overflowing the plane
	if ((direction == "a" && walk_position_sides > plane_x - 2) ||
		(direction == "d" && walk_position_sides < -plane_x + 2))
		return;

	if (direction == "a") walk_position_sides += speed;
	if (direction == "d") walk_position_sides -= speed;

	// translate all objects a "speed"
	if (direction == "a")
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], -speed, 0.0f, 0.0f);

	else
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], speed, 0.0f, 0.0f);
}

// function to walk forward and backward in Z
void walkForBack(float degreeToRotate, string direction) { // z
	// prevent from overflowing the plane
	if ((direction == "w" && walk_position_forward > plane_z - 1) ||
		(direction == "s" && walk_position_forward < -plane_z + 1)) {
		resetBodyWall();
		return;
	}

	if (direction == "w") walk_position_forward += speed;
	if (direction == "s") walk_position_forward -= speed;

	// legs new position
	Worlds[11] = PositionFinder(3.0f, 0, new float[] {-1.2f - walk_position_sides, -5.8f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, degreeToRotate, "x", "x");
	Worlds[12] = PositionFinder(3.0f, 0, new float[] {1.2f - walk_position_sides, -5.8f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, -degreeToRotate, "x",
		"x");
	Device->SetTransform(D3DTS_WORLD, &Worlds[11]);
	Objects[11]->DrawSubset(0);
	Device->SetTransform(D3DTS_WORLD, &Worlds[12]);
	Objects[12]->DrawSubset(0);

	// hands new position
	Worlds[9] = PositionFinder(3.0f, 0, new float[] {-2.85f - walk_position_sides, -2.5f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, degreeToRotate, "x", "x");
	Worlds[10] = PositionFinder(3.0f, 0, new float[] {+2.85f - walk_position_sides, -2.5f + walk_position_Yz,
		0.0f - walk_position_forward}, 3.14 / 2, -degreeToRotate, "x",
		"x");
	Device->SetTransform(D3DTS_WORLD, &Worlds[9]);
	Objects[9]->DrawSubset(0);
	Device->SetTransform(D3DTS_WORLD, &Worlds[10]);
	Objects[10]->DrawSubset(0);

	// translate all objects a "speed"
	if (direction == "w")
		for (int i = 0; i < 16; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, 0.0f, -speed);

	else
		for (int i = 0; i < 16; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, 0.0f, speed);
}

// function to walk forward and backward in Y
void walkUpDown(string direction) { // y
	// prevent from overflowing the plane
	if ((direction == "q" && walk_position_Yz > 9) || (direction == "e" && walk_position_Yz < 0.1))
		return;

	if (direction == "q") walk_position_Yz += speed;
	if (direction == "e") walk_position_Yz -= speed;

	// translate all objects a "speed"
	if (direction == "q")
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, +speed, 0.0f);

	else
		for (int i = 0; i < 14; i++)
			Worlds[i] = translate(Worlds[i], 0.0f, -speed, 0.0f);
}
*/




bool Setup()
{
	Device->CreateVertexBuffer(
		24 * sizeof(Vertex),
		0,
		Vertex::FVF,
		D3DPOOL_MANAGED,
		&VB,
		0);

	Vertex* v = 0;
	VB->Lock(0, 0, (void**)&v, 0);

	//floor
	
	v[0] = Vertex(-1000.0f , -1000.0f , -8.50f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[1] = Vertex(-1000.0f , 1000.0f , -8.50f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[2] = Vertex(1000.0f, 1000.0f , -8.50f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);

	v[3] = Vertex(-1000.0f, -1000.0f  ,-8.50f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[4] = Vertex(1000.0f, 1000.0f, -8.50f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[5] = Vertex(1000.0f , -1000.0f , -8.50f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
	// wall
	v[6] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[7] = Vertex(-7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[8] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[9] = Vertex(-7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[10] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[11] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Note: We leave gap in middle of walls for mirror

	v[12] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[13] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[14] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[15] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[16] = Vertex(7.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[17] = Vertex(7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// mirror
	v[18] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[19] = Vertex(-2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[20] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);

	v[21] = Vertex(-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[22] = Vertex(2.5f, 5.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[23] = Vertex(2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	VB->Unlock();

	//
	// Load Textures, set filters.
	//

	D3DXCreateTextureFromFile(Device, (LPCWSTR)"checker.jpg", &FloorTex);
	D3DXCreateTextureFromFile(Device, (LPCWSTR)"brick0.jpg", &WallTex);
	D3DXCreateTextureFromFile(Device, (LPCWSTR)"ice.bmp", &MirrorTex);

	Device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	Device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	D3DXCreateBox(Device, 4.60f, 2.30f, 4.80f, &Objects[0], 0);//Body
	D3DXCreateSphere(Device, 1.5f, 20, 20, &Objects[1], 0);//Head
	D3DXCreateSphere(Device, 0.1f, 10, 10, &Objects[2], 0);//Eye
	D3DXCreateSphere(Device, 0.1f, 10, 10, &Objects[3], 0);//Eye
	D3DXCreateCylinder(Device, 1.0f, 0.70f, 0.70f, 20, 20, &Objects[4], 0);//Neck
	D3DXCreateCylinder(Device, 0.70f, 0.70f, 5.10f, 20, 20, &Objects[5], 0);//Leg
	D3DXCreateCylinder(Device, 0.70f, 0.70f, 5.10f, 20, 20, &Objects[6], 0);//Leg
	D3DXCreateCylinder(Device, 1.0f, 0.70f, 0.70f, 20, 20, &Objects[7], 0);//Neck
	D3DXCreateCylinder(Device, 1.0f, 0.70f, 0.70f, 20, 20, &Objects[8], 0);//Neck
	D3DXCreateBox(Device, 4.60f, 2.30f, 0.80f, &Objects[9], 0);//body
	D3DXCreateCylinder(Device, 0.30f, 0.10f, 0.30f, 20, 20, &Objects[10], 0);//nose
	D3DXCreateBox(Device, 1.00f, 2.30f, 0.80f, &Objects[11], 0);//sholder
	D3DXCreateBox(Device, 1.00f, 2.30f, 0.80f, &Objects[12], 0);//sholder
	D3DXCreateCylinder(Device, 0.50f, 0.30f, 4.50f, 20, 20, &Objects[13], 0);//hand
	D3DXCreateCylinder(Device, 0.50f, 0.30f, 4.50f, 20, 20, &Objects[14], 0);//hand



	D3DXMatrixTranslation(&ObjWorldMatrices[0], 0.0f, 0.0f, 0.40f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1], 0.0f, 0.0f, 4.450f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2], -0.5f, 1.3f, 5.05f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], 0.50f, 1.30f, 5.050f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4], 0.0f, 0.0f, 3.050f);
	D3DXMatrixTranslation(&ObjWorldMatrices[5], -1.50f, 0.0f, -5.30f);
	D3DXMatrixTranslation(&ObjWorldMatrices[6], 1.50f, 0.0f, -5.30f);
	D3DXMatrixTranslation(&ObjWorldMatrices[7], 1.500f, 0.0f, -8.150f);
	D3DXMatrixTranslation(&ObjWorldMatrices[8], -1.50f, 0.0f, -8.1500f);
	D3DXMatrixTranslation(&ObjWorldMatrices[9], 0.0f, 0.0f, -2.40f);
	D3DXMatrixTranslation(&ObjWorldMatrices[10], 0.0f, 1.4f, 4.60f);
	D3DXMatrixTranslation(&ObjWorldMatrices[11], 2.80f, 0.0f, 2.40f);
	D3DXMatrixTranslation(&ObjWorldMatrices[12], -2.80f, 0.0f, 2.40f);
	D3DXMatrixTranslation(&ObjWorldMatrices[13], 2.90f, 0.0f, 0.20f);
	D3DXMatrixTranslation(&ObjWorldMatrices[14], -2.90f, 0.0f, 0.20f);


	Mtrls[0] = d3d::RED_MTRL;
	Mtrls[1] = d3d::WHITE_MTRL;
	Mtrls[2] = d3d::BLACK_MTRL;
	Mtrls[3] = d3d::BLACK_MTRL;
	Mtrls[4] = d3d::WHITE_MTRL;
	Mtrls[5] = d3d::WHITE_MTRL;
	Mtrls[6] = d3d::WHITE_MTRL;
	Mtrls[7] = d3d::GREEN_MTRL;
	Mtrls[8] = d3d::GREEN_MTRL;
	Mtrls[9] = d3d::BLUE_MTRL;
	Mtrls[10] = d3d::BLUE_MTRL;
	Mtrls[11] = d3d::BLUE_MTRL;
	Mtrls[12] = d3d::BLUE_MTRL;
	Mtrls[13] = d3d::GREEN_MTRL;
	Mtrls[14] = d3d::GREEN_MTRL;

	
	//light{
	D3DXVECTOR3 dir(10.0f, 10.0f, 10.0f);
	D3DXCOLOR   c = d3d::WHITE;
	D3DLIGHT9 dirLight = d3d::InitDirectionalLight(&dir, &c);
	Device->SetLight(0, &dirLight);
	Device->LightEnable(0, true);
	//}

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);


	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.50f,
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}


void Cleanup()
{
	for (int i = 0; i < 15; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}

bool Display(float timeDelta)
{
	// variables for keeping the position of the camera
	static float pos_x = 0.0f;
	static float pos_y = 10.0f;
	static float pos_z = 2.0f;

	// variables for keeping the position of the target
	static float target_x = 0.0f;
	static float target_y = 0.0f;
	static float target_z = 0.0f;

	if (Device) {
		// change the target position
		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f) // left
			target_x -= 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f) // right
			target_x += 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f) // up
			target_y += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f) // down
			target_y -= 5.0f * timeDelta;

		// change the camera position
		if (GetKeyState(VK_SHIFT) & 0x8000f) // x
			if (::GetAsyncKeyState(0x58) & 0x8000f)
				pos_x += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // x
			if (::GetAsyncKeyState(0x58) & 0x8000f)
				pos_x -= 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) & 0x8000f) // y
			if (::GetAsyncKeyState(0x59) & 0x8000f)
				pos_y += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // y
			if (::GetAsyncKeyState(0x59) & 0x8000f)
				pos_y -= 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) & 0x8000f) // z
			if (::GetAsyncKeyState(0x5a) & 0x8000f)
				pos_z += 15.0f * timeDelta;

		if (GetKeyState(VK_SHIFT) != 1) // z
			if (::GetAsyncKeyState(0x5a) & 0x8000f)
				pos_z -= 15.0f * timeDelta;
		/*
		// move forward
		if (::GetAsyncKeyState(0x57) & 0x8000f) { // w

			// walk back if last move is different from w
			if (last_move == "s") {
				if (walk_degree_forward > 0) {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward -= speed / 2.0f;
							walkForBack(-walk_degree_forward, "w");
						}

						if (walk_degree_forward > 0)
							break;
					}

				}
				else {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward += speed / 2.0f;
							walkForBack(-walk_degree_forward, "w");
						}

						if (walk_degree_forward < 0)
							break;
					}
				}

				Sleep(100);
				last_move = "w";

			}
			else {
				last_move = "w";

				if (hands_positivity)
					walk_degree_forward += speed;

				else
					walk_degree_forward -= speed;

				walkForBack(walk_degree_forward, "w");

				if (walk_degree_forward >= 3.14f / 4)
					hands_positivity = false;

				else if (walk_degree_forward <= -3.14f / 4)
					hands_positivity = true;
			}
		}

		// move backward
		if (::GetAsyncKeyState(0x53) & 0x8000f) { // s

			// walk back if last move is different from s
			if (last_move == "w") {
				if (walk_degree_forward > 0) {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward -= speed / 2.0f;
							walkForBack(walk_degree_forward, "s");
						}

						if (walk_degree_forward > 0)
							break;
					}
				}
				else {
					for (int x = 0; x < 1000000; x++) {
						if (x % 100 == 0) {
							walk_degree_forward += speed / 2.0f;
							walkForBack(walk_degree_forward, "s");
						}

						if (walk_degree_forward < 0)
							break;
					}
				}

				Sleep(100);
				last_move = "s";

			}
			else {
				last_move = "s";

				if (hands_positivity)
					walk_degree_forward += speed / 2.0f;

				else
					walk_degree_forward -= speed / 2.0f;

				walkForBack(-walk_degree_forward, "s");

				if (walk_degree_forward >= 3.14f / 6)
					hands_positivity = false;

				else if (walk_degree_forward <= -3.14f / 6)
					hands_positivity = true;
			}
		}

		//move left
		if (::GetAsyncKeyState(0x41) & 0x8000f) { // a
			walkLeftRight("a");
		}

		//move right
		if (::GetAsyncKeyState(0x44) & 0x8000f) { // d
			walkLeftRight("d");

		}
		*/
		// light on/off

		/*if (GetKeyState(VK_SHIFT) & 0x8000f) // l
			if (::GetAsyncKeyState(0x4c) & 0x8000f) {
				lightEnable = false;
				Light();
			}

		if (GetKeyState(VK_SHIFT) != 1) // l
			if (::GetAsyncKeyState(0x4c) & 0x8000f) {
				lightEnable = true;
				Light();
			}

			*/
		// setting camera
		D3DXVECTOR3 position(pos_x, pos_y, pos_z);
		D3DXVECTOR3 target(target_x, target_y, target_z);
		D3DXVECTOR3 up(0.0f, 0.0f, 1.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		Device->SetTransform(D3DTS_VIEW, &V);


		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();

		for (int i = 0; i < 15; i++)
		{
			Device->SetMaterial(&Mtrls[i]);
			// Set the world matrix that positions the object.
			Device->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);

			// Draw the object using the previously set world matrix.
			Objects[i]->DrawSubset(0);


			RenderScene();

			//RenderMirror();

		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return true;
}

LRESULT CALLBACK d3d::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
			::DestroyWindow(hwnd);
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

//
// WinMain
//
int WINAPI WinMain(HINSTANCE hinstance,
	HINSTANCE prevInstance,
	PSTR cmdLine,
	int showCmd)
{
	if (!d3d::InitD3D(hinstance,
		Width, Height, true, D3DDEVTYPE_HAL, &Device))
	{
		::MessageBox(0, (LPWSTR)"InitD3D() - FAILED", 0, 0);
		return 0;
	}

	if (!Setup())
	{
		::MessageBox(0, (LPWSTR)"Setup() - FAILED", 0, 0);
		return 0;
	}

	d3d::EnterMsgLoop(Display);

	Cleanup();

	Device->Release();

	return 0;
}
