#pragma once

#include <optional>

#include "window.h"

namespace utils::MS::window
	{
	/// <summary>
	/// I tried. I seriously tried.
	/// But there's so many ways windows can fake being visible that I've simply lost hopes. 
	/// Windows with this module will snap to some seemingly random locations, into windows that you can't see but according to Windows's APIs are visible, non-minimized and non-cloaked.
	/// </summary>
	class snap_on_drag : public module
		{
		public:
			struct create_info
				{
				using module_type = snap_on_drag;
				long snap_max_distance{16};
				};

			snap_on_drag(window::base& base, const create_info& create_info = {});

			long snap_max_distance{16};

		private:
			virtual procedure_result procedure(UINT msg, WPARAM wparam, LPARAM lparam) override;

			void get_windows() noexcept;

			std::optional<rect_t> evaluate_move();

			void eval_delta(const long& target, const long& other, long& closest_distance, long& closest);

		private:
			std::vector<rect_t> rects;
			/// <summary>
			/// Position is updated every "step".
			/// Represents where the window would be without snapping.
			/// Size is kept constant to the beginning of the move operation.
			/// </summary>
			rect_t target;
			/// <summary>
			/// Evaluated rect the previous step.
			/// Affected by snap.
			/// </summary>
			rect_t result_previous;
		};
	}

#ifdef utils_implementation
#include "snap_on_drag.cpp"
#endif