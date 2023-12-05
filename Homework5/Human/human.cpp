#include "d3dUtility.h"

const int Width = 1920;
const int Height = 1080;

IDirect3DDevice9* Device = 0;

ID3DXMesh* Objects[5] = {0};
D3DMATERIAL9 Mtrls[5];
D3DXMATRIX  Worlds[5];

D3DXMATRIX ObjWorldMatrices[5];

bool Setup()
{
	D3DXCreateBox(Device, 5.0f, 5.0f, 5.0f, &Objects[0], 0);
	D3DXCreateSphere(Device, 2.50f, 20, 20, &Objects[1], 0);
	D3DXCreateSphere(Device, 0.25f, 10, 10, &Objects[2], 0);
	D3DXCreateSphere(Device, 0.25f, 10, 10, &Objects[3], 0);
	D3DXCreateCylinder(Device, 1.5f, 1.50f,1.50f, 20, 20, &Objects[4], 0);

	D3DXMatrixTranslation(&ObjWorldMatrices[0], 0.0f, 0.0f, 0.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[1],0.0f, 0.0f, 5.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[2], 1.32f, 0.25f, 1.32f);
	D3DXMatrixTranslation(&ObjWorldMatrices[3], 0.0f,-2.0f, -2.0f);
	D3DXMatrixTranslation(&ObjWorldMatrices[4], -6.0f, -6.0f, -0.0f);



	Mtrls[0] = d3d::RED_MTRL;
	Mtrls[1] = d3d::YELLOW_MTRL;
	Mtrls[2] = d3d::RED_MTRL;
	Mtrls[3] = d3d::RED_MTRL;
	Mtrls[4] = d3d::RED_MTRL;





//light{

	D3DXVECTOR3 dir(1.0f, 0.0f, 0.25f);
	D3DXCOLOR   c = d3d::WHITE;
	D3DLIGHT9 dirLight = d3d::InitDirectionalLight(&dir, &c);



	Device->SetLight(0, &dirLight);
	Device->LightEnable(0, true);

//}

	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, false);

	D3DXVECTOR3 position(-20.0f, -20.0f, 0.0f);
	D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX V;
	D3DXMatrixLookAtLH(&V, &position, &target, &up);
	Device->SetTransform(D3DTS_VIEW, &V);

	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(
		&proj,
		D3DX_PI * 0.5f, // 90 - degree
		(float)Width / (float)Height,
		1.0f,
		1000.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	//Device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	return true;
}


void Cleanup()
{
	for (int i = 0; i < 5; i++)
		d3d::Release<ID3DXMesh*>(Objects[i]);
}


bool Display(float timeDelta)
{
	if (Device)
	{//

		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0xffffffff, 1.0f, 0);
		Device->BeginScene();

		static float angle = (3.0f * D3DX_PI) / 2.0f;
		static float height = 5.0f;

		if (::GetAsyncKeyState(VK_LEFT) & 0x8000f)
			angle -= 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_RIGHT) & 0x8000f)
			angle += 0.5f * timeDelta;

		if (::GetAsyncKeyState(VK_UP) & 0x8000f)
			height += 5.0f * timeDelta;

		if (::GetAsyncKeyState(VK_DOWN) & 0x8000f)
			height -= 5.0f * timeDelta;


		D3DXVECTOR3 position(cosf(angle) * 7.0f, height, sinf(angle) * 7.0f);
		D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
		D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);

		Device->SetTransform(D3DTS_VIEW, &V);




		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
		Device->BeginScene();


		for (int i = 0; i < 5; i++)
		{
			Device->SetMaterial(&Mtrls[i]);
			// Set the world matrix that positions the object.
			Device->SetTransform(D3DTS_WORLD, &ObjWorldMatrices[i]);


			// Draw the object using the previously set world matrix.
			Objects[i]->DrawSubset(0);
		}
		//for (int i = 0; i < 4; i++)
		//{
		
		//	Device->SetMaterial(&Mtrls[i]);
		//	Device->SetTransform(D3DTS_WORLD, &Worlds[i]);
		//	Objects[i]->DrawSubset(0);
		//}
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