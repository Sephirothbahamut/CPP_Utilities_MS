#include <string>
#include <chrono>
#include <thread>

#include <utils/MS/windows.h>
#include <utils/MS/window/window.h>
#include <utils/MS/window/style.h>
#include <utils/MS/window/regions.h>
#include <utils/MS/window/input/passthrough.h>

#include <utils/graphics/opengl/window.h>

#include "examples.h"

static void body()
	{//https://www.nvidia.com/content/gtc/documents/1055_gtc09.pdf

	utils::MS::window::initializer window_initializer;

	utils::MS::window::base window
		{
		utils::MS::window::base::create_info{.position{{512, 512}}, .size{{512u, 512u}}},
		utils::MS::window::style::create_info
			{
			.transparency{utils::MS::window::style::transparency_t::DWM_transparent},
			.borders{utils::MS::window::style::value_t::disable},
			.alpha{127}
			},
		utils::MS::window::input::passthrough::create_info{},
		//utils::MS::window::regions::create_info{.default_hit_type{utils::MS::window::hit_type::client}},
		//utils::MS::window::resizable_edge::create_info{.thickness{8}},
		utils::graphics::opengl::window::create_info{}
		};

	std::cout << GetWindowLong(window.get_handle(), GWL_EXSTYLE) << std::endl;
	std::cout << ((GetWindowLong(window.get_handle(), GWL_EXSTYLE) & (DWORD{WS_EX_TRANSPARENT} | DWORD{WS_EX_LAYERED})) == (DWORD{WS_EX_TRANSPARENT} | DWORD{WS_EX_LAYERED})) << std::endl;
	
	std::chrono::time_point start{std::chrono::high_resolution_clock::now()};

	auto opengl_window_module{window.get_module_ptr<utils::graphics::opengl::window>()};
	opengl_window_module->render = [start]()
		{
		constexpr std::chrono::milliseconds period{5000};
		constexpr float triangle_alpha{1.f};

		const auto delta_time{std::chrono::high_resolution_clock::now() - start};

		glClearColor(0.f, 0.f, 0.f, 0.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glPushMatrix();

		std::chrono::duration<float, std::ratio<1, 1000>> elapsed_seconds_float{delta_time};

		utils::math::angle::degf hsv_angle_increment{elapsed_seconds_float.count() * .2f};
		

		glColor4f(0, 1, 1, 1.f);
		glBegin(GL_TRIANGLE_FAN);
			glColor4f (1.f, 1.f, 1.f, 0.f);
			glVertex3f(0.f, 0.f, 0.f);

			for (float i = 0.f; i <= 1.1f; i += 1.f / 1000.f)
				{
				utils::math::angle::degf angle{i * utils::math::angle::degf::full_angle};
				angle.value++;
				angle.clamp_self();

				auto vec{utils::math::vec2f::from(angle, 1.f)};

				utils::graphics::colour::hsv_f hsv{.h{(angle + hsv_angle_increment).clamp_self()}, .s{1.f}, .v{1.f}};
				auto rgb{hsv.rgb()};

				glColor4f(rgb.r, rgb.g, rgb.b, 1.f);
				glVertex3f(vec.x, vec.y, 0.f);
				}
		glEnd();

		glPopMatrix();
		glFlush();
		};

	SetWindowPos(window.get_handle(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	UpdateWindow(window.get_handle());
	//SetWindowLong(window.get_handle(), GWL_EXSTYLE, GetWindowLong(window.get_handle(), GWL_EXSTYLE) | WS_EX_TRANSPARENT | WS_EX_LAYERED);
	//SetLayeredWindowAttributes(window.get_handle(), 0, 255, LWA_ALPHA);

	std::chrono::milliseconds min_ms_per_frame{500};

	window.show();
	while (window.is_open())
		{
		std::chrono::time_point frame_start{std::chrono::high_resolution_clock::now()};

		while(window.poll_event());

		opengl_window_module->draw();
		
		POINT mouse_coords;
		if (GetCursorPos(&mouse_coords))
			{
			utils::math::vec2l half_size{utils::math::vec2l{window.window_rect.size()} / 2};

			utils::math::rect<long> new_rect
				{
				.ll{mouse_coords.x - half_size.x},
				.up{mouse_coords.y - half_size.y},
				.rr{mouse_coords.x + half_size.x},
				.dw{mouse_coords.y + half_size.y},
				};

			window.window_rect = new_rect;
			}

		std::chrono::time_point frame_end{std::chrono::high_resolution_clock::now()};
		auto elapsed{frame_start - frame_end};
		if (elapsed < min_ms_per_frame)
			{
			std::this_thread::sleep_for(elapsed - min_ms_per_frame);
			}
		}
	}

void example::fancy_opengl_wheel()
	{
	try { body(); }
	catch (const std::system_error& e) { ::MessageBoxA(nullptr, e.what(), "Unhandled Exception", MB_OK | MB_ICONERROR); }
	}