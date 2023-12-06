#pragma once

#include <GL/GL.h>

#include <utils/oop/crtp.h>

#include "../../MS/window/window.h"

namespace utils::graphics::opengl
	{
	class window : public utils::MS::window::module
		{
		public:
			struct create_info
				{
				using module_type = opengl::window;
				//bool window_transparency_support{false};
				};

			window(utils::MS::window::base& base, const create_info& create_info = {}) :
				utils::MS::window::module{base}
				{
				CreateHGLRC(); 
				HDC hdc{GetDC(get_base().get_handle())};
				wglMakeCurrent(hdc, m_hrc);
				initSC();
				resizeSC(get_base().client_rect.width(), get_base().client_rect.height());
				ReleaseDC(get_base().get_handle(), hdc);
				}

			std::function<void()> render;

			void draw()
				{
				HDC hdc{GetDC(get_base().get_handle())};
				wglMakeCurrent(hdc, m_hrc);

				render();

				SwapBuffers(hdc);
				ReleaseDC(get_base().get_handle(), hdc);
				}

		protected:
			HDC hDC;
			HGLRC m_hrc;
			HDC pdcDIB;
			HBITMAP hbmpDIB;
			void* bmp_cnt{nullptr};
			int cxDIB{0};
			int cyDIB{0};
			BITMAPINFOHEADER BIH;

			virtual utils::MS::window::procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override
				{
				switch (msg)
					{
					case WM_ERASEBKGND: return utils::MS::window::procedure_result::stop(0);

					case WM_DESTROY:
						if (m_hrc)
							{
							wglMakeCurrent(NULL, NULL);
							wglDeleteContext(m_hrc);
							}
						break;

					case WM_MOVING:
						{
						if (render) { draw(); }
						}
						break;

					case WM_SIZE:
						{
						int w{LOWORD(lparam)};
						int h{HIWORD(lparam)};
						
						resizeSC(w, h);
						if (render) { draw(); }
						}
						break;
					}

				return utils::MS::window::procedure_result::next();
				}

			BOOL initSC()
				{
				//glEnable(GL_ALPHA_TEST);
				glEnable(GL_DEPTH_TEST);
				//glEnable(GL_COLOR_MATERIAL);

				//glEnable(GL_LIGHTING);
				//glEnable(GL_LIGHT0);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				glClearColor(0, 0, 0, 0);

				return 0;
				}

			void resizeSC(int width, int height)
				{
				glViewport(0, 0, width, height);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();

				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
				}

			BOOL CreateHGLRC()
				{
				DWORD dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;

				PIXELFORMATDESCRIPTOR pfd
					{
					.nSize          {sizeof(PIXELFORMATDESCRIPTOR)},
					.nVersion       {1},
					.dwFlags        
						{
						PFD_DRAW_TO_WINDOW      |         // Format Must Support Window
						PFD_SUPPORT_OPENGL      |         // Format Must Support OpenGL
						PFD_SUPPORT_COMPOSITION |         // Format Must Support Composition
						PFD_DOUBLEBUFFER                  // Must Support Double Buffering
						},
					.iPixelType     {PFD_TYPE_RGBA},
					.cColorBits     {32},
					.cRedBits       {0},
					.cRedShift      {0},
					.cGreenBits     {0},
					.cGreenShift    {0},
					.cBlueBits      {0},
					.cBlueShift     {0},
					.cAlphaBits     {8},
					.cAlphaShift    {0},
					.cAccumBits     {0},
					.cAccumRedBits  {0},
					.cAccumGreenBits{0},
					.cAccumBlueBits {0},
					.cAccumAlphaBits{0},
					.cDepthBits     {24},
					.cStencilBits   {8},
					.cAuxBuffers    {0},
					.iLayerType     {PFD_MAIN_PLANE},
					.bReserved      {0},
					.dwLayerMask    {0},
					.dwVisibleMask  {0},
					.dwDamageMask   {0},
					};
				
				HDC hdc = GetDC(get_base().get_handle());
				int PixelFormat = ChoosePixelFormat(hdc, &pfd);
				if (PixelFormat == 0) {
					assert(0);
					return FALSE;
					}

				BOOL bResult = SetPixelFormat(hdc, PixelFormat, &pfd);
				if (bResult == FALSE) {
					assert(0);
					return FALSE;
					}

				m_hrc = wglCreateContext(hdc);
				if (!m_hrc) {
					assert(0);
					return FALSE;
					}

				ReleaseDC(get_base().get_handle(), hdc);

				return TRUE;
				}
		};
	}