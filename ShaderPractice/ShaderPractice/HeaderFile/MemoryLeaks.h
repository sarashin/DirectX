/*
	@file	MemoryLeaks.h
	@date	2018/02/11
	@author	番場 宥輝
	@brief	メモリリーク特定マクロ定義ヘッダー
*/
#pragma once
#if defined _DEBUG || defined DEBUG
#include <crtdbg.h>
#define	new	new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif