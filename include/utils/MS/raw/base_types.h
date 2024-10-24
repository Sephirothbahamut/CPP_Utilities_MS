#pragma once

//namespace utils::MS
//	{
//	using DWORD    =  unsigned long ;
//	using BOOL     =  int           ;
//	using BYTE     =  unsigned char ;
//	using WORD     =  unsigned short;
//	using FLOAT    =  float         ;
//	using PFLOAT   =  FLOAT        *;
//	using PBOOL    =  BOOL         *;
//	using LPBOOL   =  BOOL         *;
//	using PBYTE    =  BYTE         *;
//	using LPBYTE   =  BYTE         *;
//	using PINT     =  int          *;
//	using LPINT    =  int          *;
//	using PWORD    =  WORD         *;
//	using LPWORD   =  WORD         *;
//	using LPLONG   =  long         *;
//	using PDWORD   =  DWORD        *;
//	using LPDWORD  =  DWORD        *;
//	using LPVOID   =  void         *;
//	using LPCVOID  =  const void   *;
//
//	using INT      = int            ;
//	using UINT     = unsigned int   ;
//	using PUINT    = unsigned int  *;
//
//
//	using INT_PTR    =          __int64 ;
//	using PINT_PTR   =          __int64*;
//	using UINT_PTR   = unsigned __int64 ;
//	using PUINT_PTR  = unsigned __int64*;
//
//	using LONG_PTR   =          __int64 ;
//	using PLONG_PTR  =          __int64*;
//	using ULONG_PTR  = unsigned __int64 ;
//	using PULONG_PTR = unsigned __int64*;
//
//	using WPARAM  = UINT_PTR;
//	using LPARAM  = LONG_PTR;
//	using LRESULT = LONG_PTR;
//	}

//TODO put in dedicated windowing subfolder
// 
//struct HWND__;
//
//namespace utils::MS
//	{
//	using HWND = HWND__*;
//	}
//namespace utils::MS::window_styles
//	{
//	inline constexpr DWORD ws_overlapped  {0x00000000l};
//	inline constexpr DWORD ws_popup       {0x80000000l};
//	inline constexpr DWORD ws_child       {0x40000000l};
//	inline constexpr DWORD ws_minimize    {0x20000000l};
//	inline constexpr DWORD ws_visible     {0x10000000l};
//	inline constexpr DWORD ws_disabled    {0x08000000l};
//	inline constexpr DWORD ws_clipsiblings{0x04000000l};
//	inline constexpr DWORD ws_clipchildren{0x02000000l};
//	inline constexpr DWORD ws_maximize    {0x01000000l};
//	inline constexpr DWORD ws_caption     {0x00c00000l}; // ws_border | ws_dlgframe
//	inline constexpr DWORD ws_border      {0x00800000l};
//	inline constexpr DWORD ws_dlgframe    {0x00400000l};
//	inline constexpr DWORD ws_vscroll     {0x00200000l};
//	inline constexpr DWORD ws_hscroll     {0x00100000l};
//	inline constexpr DWORD ws_sysmenu     {0x00080000l};
//	inline constexpr DWORD ws_thickframe  {0x00040000l};
//	inline constexpr DWORD ws_group       {0x00020000l};
//	inline constexpr DWORD ws_tabstop     {0x00010000l};
//
//	inline constexpr DWORD ws_minimizebox {0x00020000l};
//	inline constexpr DWORD ws_maximizebox {0x00010000l};
//
//	inline constexpr DWORD ws_tiled       {ws_overlapped};
//	inline constexpr DWORD ws_iconic      {ws_minimize};
//	inline constexpr DWORD ws_sizebox     {ws_thickframe};
//
//	inline constexpr DWORD ws_overlappedwindow{ws_overlapped
//	                                          |ws_caption
//	                                          |ws_sysmenu
//	                                          |ws_thickframe
//	                                          |ws_minimizebox
//	                                          |ws_maximizebox};
//	
//	inline constexpr DWORD ws_tiledwindow {ws_overlappedwindow};
//
//	inline constexpr DWORD ws_popupwindow     {ws_popup
//	                                          |ws_border
//	                                          |ws_sysmenu};
//
//	inline constexpr DWORD ws_childwindow{ws_child};
//
//
//	inline constexpr DWORD ws_ex_dlgmodalframe {0x00000001l};
//	inline constexpr DWORD ws_ex_noparentnotify{0x00000004l};
//	inline constexpr DWORD ws_ex_topmost       {0x00000008l};
//	inline constexpr DWORD ws_ex_acceptfiles   {0x00000010l};
//	inline constexpr DWORD ws_ex_transparent   {0x00000020l};
//	inline constexpr DWORD ws_ex_mdichild      {0x00000040l};
//	inline constexpr DWORD ws_ex_toolwindow    {0x00000080l};
//	inline constexpr DWORD ws_ex_windowedge    {0x00000100l};
//	inline constexpr DWORD ws_ex_clientedge    {0x00000200l};
//	inline constexpr DWORD ws_ex_contexthelp   {0x00000400l};
//	}
//
//namespace utils::MS::nchittest_results
//	{
//	inline constexpr LRESULT hterror      {(-2)};
//	inline constexpr LRESULT httransparent{(-1)};
//	inline constexpr LRESULT htnowhere    {0};
//	inline constexpr LRESULT htclient     {1};
//	inline constexpr LRESULT htcaption    {2};
//	inline constexpr LRESULT htsysmenu    {3};
//	inline constexpr LRESULT htgrowbox    {4};
//	inline constexpr LRESULT htsize       {htgrowbox};
//	inline constexpr LRESULT htmenu       {5};
//	inline constexpr LRESULT hthscroll    {6};
//	inline constexpr LRESULT htvscroll    {7};
//	inline constexpr LRESULT htminbutton  {8};
//	inline constexpr LRESULT htmaxbutton  {9};
//	inline constexpr LRESULT htleft       {10};
//	inline constexpr LRESULT htright      {11};
//	inline constexpr LRESULT httop        {12};
//	inline constexpr LRESULT httopleft    {13};
//	inline constexpr LRESULT httopright   {14};
//	inline constexpr LRESULT htbottom     {15};
//	inline constexpr LRESULT htbottomleft {16};
//	inline constexpr LRESULT htbottomright{17};
//	inline constexpr LRESULT htborder     {18};
//	inline constexpr LRESULT htreduce     {htminbutton};
//	inline constexpr LRESULT htzoom       {htmaxbutton};
//	inline constexpr LRESULT htsizefirst  {htleft};
//	inline constexpr LRESULT htsizelast   {htbottomright};
//	inline constexpr LRESULT htobject     {19};
//	inline constexpr LRESULT htclose      {20};
//	inline constexpr LRESULT hthelp       {21};
//	}