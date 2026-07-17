#include "../Precompiled.h"
#include "../gui/new/new_menu.h"
#include "../gui/new/new_imagess.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../../ext/stb/stb_image.h"

// Load texture from raw PNG/JPG memory using stb_image + native D3D11
static bool LoadTextureFromMemory(ID3D11Device* pDevice, const void* pData, size_t nSize, ID3D11ShaderResourceView** ppSRV)
{
	if (!pData || !nSize || !ppSRV) return false;
	int w = 0, h = 0, ch = 0;
	unsigned char* px = stbi_load_from_memory(reinterpret_cast<const stbi_uc*>(pData), static_cast<int>(nSize), &w, &h, &ch, 4);
	if (!px) return false;
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = w; desc.Height = h; desc.MipLevels = 1; desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	D3D11_SUBRESOURCE_DATA init = {};
	init.pSysMem = px; init.SysMemPitch = w * 4;
	ID3D11Texture2D* pTex = nullptr;
	HRESULT hr = pDevice->CreateTexture2D(&desc, &init, &pTex);
	stbi_image_free(px);
	if (FAILED(hr)) return false;
	D3D11_SHADER_RESOURCE_VIEW_DESC srv = {};
	srv.Format = desc.Format;
	srv.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv.Texture2D.MipLevels = 1;
	hr = pDevice->CreateShaderResourceView(pTex, &srv, ppSRV);
	pTex->Release();
	return SUCCEEDED(hr);
}

void Draw::Setup(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	ImGui::SetAllocatorFunctions(
		[](size_t s, void*) -> void* {return Interfaces::m_pMemAlloc->Alloc(s);},
		[](void* p, void*) -> void {Interfaces::m_pMemAlloc->Free(p);}
	);

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(Input::m_hWindow);
	ImGui_ImplDX11_Init(pDevice, pDeviceContext);

	if (pic::aimbot == nullptr) LoadTextureFromMemory(pDevice, aimbot, sizeof(aimbot), &pic::aimbot);
	if (pic::visuals == nullptr) LoadTextureFromMemory(pDevice, visuals, sizeof(visuals), &pic::visuals);
	if (pic::skins == nullptr) LoadTextureFromMemory(pDevice, skins, sizeof(skins), &pic::skins);
	if (pic::settings == nullptr) LoadTextureFromMemory(pDevice, settings, sizeof(settings), &pic::settings);

	if (pic::combo_widget == nullptr) LoadTextureFromMemory(pDevice, combo_widget, sizeof(combo_widget), &pic::combo_widget);
	if (pic::input_widget == nullptr) LoadTextureFromMemory(pDevice, input_widget, sizeof(input_widget), &pic::input_widget);

	if (pic::menu_settings_icon == nullptr) LoadTextureFromMemory(pDevice, menu_settings_icon, sizeof(menu_settings_icon), &pic::menu_settings_icon);

	if (pic::circle_success == nullptr) LoadTextureFromMemory(pDevice, circle_success, sizeof(circle_success), &pic::circle_success);
	if (pic::circle_error == nullptr) LoadTextureFromMemory(pDevice, circle_error, sizeof(circle_error), &pic::circle_error);

	blurEffect.Initialize(pDevice, pDeviceContext);

	m_bInitialized = true;
}

void Draw::Destroy( )
{
	m_bInitialized = false;

	// shutdown imgui directx11 renderer binding
	ImGui_ImplDX11_Shutdown();

	// shutdown imgui win32 platform binding
	ImGui_ImplWin32_Shutdown();

	// destroy imgui context
	ImGui::DestroyContext();

	// destroy render target
	Interfaces::DestroyRenderTarget();
}

void Draw::RenderDrawData( ImDrawList* pDrawList )
{
	std::unique_lock<std::shared_mutex> lock( drawMutex );

	if (vecSafeDrawData.empty( ))
		return;

	for (const auto& data : vecSafeDrawData)
	{
		if (!data.pObject.has_value( ))
			continue;

		switch (data.nType)
		{
			case EDrawType::LINE:
			{
				const auto& pObject = std::any_cast< LineObject_t >( data.pObject );
				pDrawList->AddLine( pObject.vecStart, pObject.vecEnd, pObject.colLine, pObject.flThickness );
				break;
			}
			case EDrawType::RECT:
			{
				const auto& pObject = std::any_cast< RectObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_RECT_ALIGNED)
					AddDrawListRect( pDrawList,
						{ floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
						{ floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
						pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness );
				else
					AddDrawListRect( pDrawList, pObject.vecMin, pObject.vecMax, pObject.colRect, pObject.uFlags, pObject.colOutline, pObject.flRounding, pObject.flThickness );

				break;
			}
			case EDrawType::RECT_MULTICOLOR:
			{
				const auto& pObject = std::any_cast< RectMultiColorObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_RECT_ALIGNED)
					AddDrawListRectMultiColor( pDrawList,
						{ floor( pObject.vecMin.x ), floor( pObject.vecMin.y ) },
						{ floor( pObject.vecMax.x ), floor( pObject.vecMax.y ) },
						pObject.colUpperLeft, pObject.colUpperRight,
						pObject.colBottomLeft, pObject.colBottomRight,
						pObject.uFlags, pObject.colOutline );
				else
					AddDrawListRectMultiColor( pDrawList,
						pObject.vecMin,
						pObject.vecMax,
						pObject.colUpperLeft, pObject.colUpperRight,
						pObject.colBottomLeft, pObject.colBottomRight,
						pObject.uFlags, pObject.colOutline );

				break;
			}
			case EDrawType::CIRCLE:
			{
				const auto& pObject = std::any_cast< CircleObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_CIRCLE_FILLED)
					pDrawList->AddCircleFilled( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments );
				else
					pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius, pObject.colCircle, pObject.nSegments, pObject.flThickness );

				if (pObject.uFlags & DRAW_CIRCLE_OUTLINE)
					pDrawList->AddCircle( pObject.vecCenter, pObject.flRadius + 1.0f, pObject.colOutline, pObject.nSegments, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::CIRCLE_FILLED:
			{
				const auto& pObject = std::any_cast<CircleFilledObject_t>(data.pObject);
				pDrawList->AddCircleFilled(pObject.vecCenter, pObject.flRadius, pObject.uColor, 32);
				break;
			}
			case EDrawType::GRADIENT_CIRCLE:
			{
				const auto& pObject = std::any_cast<GradientCircleObject_t>(data.pObject);

				Draw::CircleGradient( pDrawList, pObject.vecCenter, pObject.flRadius, pObject.colFrom, pObject.colTo );
				break;
			}
			case EDrawType::RAINBOW_CIRCLE:
			{
				const auto& pObject = std::any_cast<RainbowCircleObject_t>(data.pObject);

				Draw::CircleRainbow(pDrawList, pObject.vecCenter, pObject.flRadius, pObject.bRotate);
				break;
			}
			case EDrawType::TRIANGLE:
			{
				const auto& pObject = std::any_cast< TriangleObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_TRIANGLE_FILLED)
					pDrawList->AddTriangleFilled( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle );
				else
					pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colTriangle, pObject.flThickness );

				if (pObject.uFlags & DRAW_TRIANGLE_OUTLINE)
					pDrawList->AddTriangle( pObject.vecFirst, pObject.vecSecond, pObject.vecThird, pObject.colOutline, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::POLYGON:
			{
				const auto& pObject = std::any_cast< PolygonObject_t >( data.pObject );

				if (pObject.uFlags & DRAW_POLYGON_FILLED)
					pDrawList->AddConvexPolyFilled( pObject.vecPoints.data( ), static_cast< int >( pObject.vecPoints.size( ) ), pObject.colPolygon );
				else
					pDrawList->AddPolyline( pObject.vecPoints.data( ), static_cast<int>( pObject.vecPoints.size( ) ), pObject.colPolygon, pObject.bClosed, pObject.flThickness );

				if (pObject.uFlags & DRAW_POLYGON_OUTLINE)
					pDrawList->AddPolyline( pObject.vecPoints.data( ), static_cast< int >( pObject.vecPoints.size( ) ), pObject.colOutline, pObject.bClosed, pObject.flThickness + 1.0f );

				break;
			}
			case EDrawType::TEXT:
			{
				const auto& pObject = std::any_cast< TextObject_t >( data.pObject );
				AddDrawListText( pDrawList, pObject.pFont, pObject.flFontSize, pObject.vecPosition, pObject.szText, pObject.colText, pObject.uFlags, pObject.colOutline );
				break;
			}
			case EDrawType::IMAGE:
			{
				const auto& pObject = std::any_cast< ImageObject_t >( data.pObject );
				pDrawList->AddImageRounded( pObject.pTexture, pObject.vecMin, pObject.vecMax, ImVec2( 0, 0 ), ImVec2( 1, 1 ), pObject.colImage, pObject.flRounding );
				break;
			}
			case EDrawType::ARC:
			{
				const auto& pObject = std::any_cast< ArcObject_t >( data.pObject );
				pDrawList->PathArcTo( pObject.vecPosition, pObject.flRadius, M_DEG2RAD( pObject.flMinAngle ), M_DEG2RAD( pObject.flMaxAngle ), 32 );
				pDrawList->PathStroke( pObject.colFill, false, pObject.flThickness );
				break;
			}
			case EDrawType::RING:
			{
				Vector prevPos;
				ImVec2 pervScreenPos, screenPos;
				const auto& pObject = std::any_cast< RingObject_t >( data.pObject );
				float step = ( float )M_PI * 2.0f / pObject.nSegments;

				for (float a = 0; a < ( M_PI * 2.0f ); a += step)
				{
					Vector start( pObject.flRadius * cosf( a ) + pObject.vecCenter.x, pObject.flRadius * sinf( a ) + pObject.vecCenter.y, pObject.vecCenter.z );
					Vector end( pObject.flRadius * cosf( a + step ) + pObject.vecCenter.x, pObject.flRadius * sinf( a + step ) + pObject.vecCenter.y, pObject.vecCenter.z );

					ImVec2 start2d, end2d;
					ImVec2 start22d( start2d.x, start2d.y );
					ImVec2 end22d( end2d.x, end2d.y );
					if (WorldToScreen( start, start2d ) &&
						WorldToScreen( end, end2d ))
					{
						start22d.x = start2d.x;
						start22d.y = start2d.y;

						end22d.x = end2d.x;
						end22d.y = end2d.y;

						pDrawList->AddLine( ImVec2( start22d.x, start22d.y ), ImVec2( end22d.x, end22d.y ), pObject.colCircle, pObject.flThickness );
					}
				}
				break;
			}
			default:
				break;
		}
	}
}

void Draw::ClearDrawData( )
{
	if (!vecDrawData.empty( ))
		vecDrawData.clear( );
}

void Draw::SwapDrawData( )
{
	std::unique_lock<std::shared_mutex> lock( drawMutex );
	vecDrawData.swap( vecSafeDrawData );
}

#pragma region draw_render

void Draw::AddLine( const ImVec2& vecStart, const ImVec2& vecEnd, const Color& colLine, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::LINE, std::make_any<LineObject_t>( LineObject_t{ vecStart, vecEnd, colLine.GetU32( ), flThickness } ) );
}

void Draw::AddRect( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colRect, unsigned int uFlags, const Color& colOutline, float flRounding, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::RECT, std::make_any<RectObject_t>( RectObject_t{ vecMin, vecMax, colRect.GetU32( ), uFlags, colOutline.GetU32( ), flRounding, flThickness } ) );
}

void Draw::AddRectMultiColor( const ImVec2& vecMin, const ImVec2& vecMax, const Color& colUpperLeft, const Color& colUpperRight, const Color& colBottomLeft, const Color& colBottomRight, unsigned int uFlags, const Color& colOutline )
{
	vecDrawData.emplace_back( EDrawType::RECT_MULTICOLOR, std::make_any<RectMultiColorObject_t>( RectMultiColorObject_t{ vecMin, vecMax, colUpperLeft.GetU32( ), colUpperRight.GetU32( ), colBottomLeft.GetU32( ), colBottomRight.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
}

void Draw::AddCircle( const ImVec2& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, const Color& colOutline, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::CIRCLE, std::make_any<CircleObject_t>( CircleObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, colOutline.GetU32( ), flThickness } ) );
}

void Draw::AddCircleFilled(const ImVec2& vecCenter, float flRadius, const Color& col)
{
	vecDrawData.emplace_back(EDrawType::CIRCLE_FILLED, std::make_any<CircleFilledObject_t>(CircleFilledObject_t{ vecCenter, flRadius, col.GetU32() }));
}

void Draw::AddGradientCircle(const ImVec2& vecCenter, float flRadius, const Color& colFrom, const Color& colTo)
{
	vecDrawData.emplace_back(EDrawType::GRADIENT_CIRCLE, std::make_any<GradientCircleObject_t>(GradientCircleObject_t{ vecCenter, flRadius, colFrom, colTo }));
}

void Draw::AddRainbowCircle(const ImVec2& vecCenter, float flRadius, bool bRotate)
{
	vecDrawData.emplace_back(EDrawType::RAINBOW_CIRCLE, std::make_any<RainbowCircleObject_t>(RainbowCircleObject_t{ vecCenter, flRadius, bRotate }));
}

void Draw::AddRing( const Vector& vecCenter, float flRadius, const Color& colCircle, int nSegments, unsigned int uFlags, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::RING, std::make_any<RingObject_t>( RingObject_t{ vecCenter, flRadius, colCircle.GetU32( ), nSegments, uFlags, flThickness } ) );
}

void Draw::AddTriangle( const ImVec2& vecFirst, const ImVec2& vecSecond, const ImVec2& vecThird, const Color& colTriangle, unsigned int uFlags, const Color& colOutline, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::TRIANGLE, std::make_any<TriangleObject_t>( TriangleObject_t{ vecFirst, vecSecond, vecThird, colTriangle.GetU32( ), uFlags, colOutline.GetU32( ), flThickness } ) );
}

void Draw::AddPolygon( std::vector<ImVec2>& vecPoints, const Color& colPolygon, unsigned int uFlags, const Color& colOutline, bool bClosed, float flThickness )
{
	vecDrawData.emplace_back( EDrawType::POLYGON, std::make_any<PolygonObject_t>( PolygonObject_t{ std::move( vecPoints ), colPolygon.GetU32( ), uFlags, colOutline.GetU32( ), bClosed, flThickness } ) );
}

void Draw::AddText( ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, const Color& colText, unsigned int uFlags, const Color& colOutline )
{
	if (pFont->ContainerAtlas == nullptr)
		return;

	// check is only one flag selected
	IM_ASSERT( ImIsPowerOfTwo( uFlags == DRAW_TEXT_NONE || uFlags & ( DRAW_TEXT_DROPSHADOW | DRAW_TEXT_OUTLINE ) ) );

	vecDrawData.emplace_back( EDrawType::TEXT, std::make_any<TextObject_t>( TextObject_t{ pFont, flFontSize, vecPosition, szText, colText.GetU32( ), uFlags, colOutline.GetU32( ) } ) );
}

ImU32 InterpolateColor(ImU32 colA, ImU32 colB, float t)
{
	int rA = (colA >> IM_COL32_R_SHIFT) & 0xFF;
	int gA = (colA >> IM_COL32_G_SHIFT) & 0xFF;
	int bA = (colA >> IM_COL32_B_SHIFT) & 0xFF;
	int aA = (colA >> IM_COL32_A_SHIFT) & 0xFF;

	int rB = (colB >> IM_COL32_R_SHIFT) & 0xFF;
	int gB = (colB >> IM_COL32_G_SHIFT) & 0xFF;
	int bB = (colB >> IM_COL32_B_SHIFT) & 0xFF;
	int aB = (colB >> IM_COL32_A_SHIFT) & 0xFF;

	int r = (int)(rA + t * (rB - rA));
	int g = (int)(gA + t * (gB - gA));
	int b = (int)(bA + t * (bB - bA));
	int a = (int)(aA + t * (aB - aA));

	return IM_COL32(r, g, b, a);
}

void Draw::AddDrawListGradientLine(ImDrawList* pDrawList, const ImVec2& vecStart, const ImVec2& vecEnd, ImU32 colFrom, ImU32 colTo, int nSegments, float flThicknessStart, float flThicknessEnd)
{
	if (nSegments < 1)
		return;

	ImVec2 segment = (vecEnd - vecStart) / nSegments;

	for (int i = 0; i < nSegments; ++i)
	{
		ImVec2 start = vecStart + segment * i;
		ImVec2 end = vecStart + segment * (i + 1);
		float t = (float)i / (float)nSegments;
		ImU32 colStart = InterpolateColor(colFrom, colTo, t);
		ImU32 colEnd = InterpolateColor(colFrom, colTo, (float)(i + 1) / (float)nSegments);
		pDrawList->AddLine(start, end, colStart, flThicknessStart);
	}
}

void Draw::AddText( const ImVec2& vecPosition, const std::string& szText, const Color& colText, int iFlags, const Color& colOutline )
{
	AddText( nullptr, 0.f, vecPosition, szText, colText, iFlags, colOutline );
}

void Draw::AddImage( ImTextureID pTexture, const ImVec2& vecMin, const ImVec2& vecMax, const Color& colImage, float flRounding )
{
	vecDrawData.emplace_back( EDrawType::IMAGE, std::make_any<ImageObject_t>( ImageObject_t{ pTexture, vecMin, vecMax, colImage.GetU32( ), flRounding } ) );
}

void Draw::AddArcFilled( const ImVec2& vecPosition, float flRadius, float flMinAngle, float flMaxAngle, const Color& colFill, float flThickness ) {
	vecDrawData.emplace_back( EDrawType::ARC, std::make_any<ArcObject_t>( ArcObject_t{ vecPosition,flRadius,flMinAngle,flMaxAngle,colFill.GetU32( ),flThickness } ) );
}
#pragma endregion

#pragma region draw_bindings
void Draw::AddDrawListRect( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colRect, unsigned int uFlags, ImU32 colOutline, float flRounding, float flThickness )
{
	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilled( vecMin, vecMax, colRect, flRounding );
	else
		pDrawList->AddRect( vecMin, vecMax, colRect, flRounding, 0, flThickness );

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline, flRounding, 0, 1.0f );

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline, flRounding,0, 1.0f );
}

void Draw::AddDrawListRectMultiColor( ImDrawList* pDrawList, const ImVec2& vecMin, const ImVec2& vecMax, ImU32 colUpperLeft, ImU32 colUpperRight, ImU32 colBottomLeft, ImU32 colBottomRight, unsigned int uFlags, ImU32 colOutline )
{
	if (uFlags & DRAW_RECT_FILLED)
		pDrawList->AddRectFilledMultiColor( vecMin, vecMax, colUpperLeft, colUpperRight, colBottomRight, colBottomLeft );
	else
	{
		pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMax.x, vecMin.y + 1.f ), colUpperLeft, colUpperRight, colUpperRight, colUpperLeft ); // top
		pDrawList->AddRectFilledMultiColor( ImVec2( vecMin.x, vecMax.y ), vecMax - ImVec2( 0, 1.f ), colBottomLeft, colBottomRight, colBottomRight, colBottomLeft ); // bottom

		pDrawList->AddRectFilledMultiColor( vecMin, ImVec2( vecMin.x + 1.f, vecMax.y ), colUpperLeft, colUpperLeft, colBottomLeft, colBottomLeft ); // left
		pDrawList->AddRectFilledMultiColor( ImVec2( vecMax.x, vecMin.y ), vecMax - ImVec2( 1.f, 0 ), colUpperRight, colUpperRight, colBottomRight, colBottomRight ); // right
	}

	if (uFlags & DRAW_RECT_BORDER)
		pDrawList->AddRect( vecMin + ImVec2( 1.0f, 1.0f ), vecMax - ImVec2( 1.0f, 1.0f ), colOutline );

	if (uFlags & DRAW_RECT_OUTLINE)
		pDrawList->AddRect( vecMin - ImVec2( 1.0f, 1.0f ), vecMax + ImVec2( 1.0f, 1.0f ), colOutline );
}

void Draw::AddDrawListText( ImDrawList* pDrawList, ImFont* pFont, float flFontSize, const ImVec2& vecPosition, const std::string& szText, ImU32 colText, unsigned int uFlags, ImU32 colOutline )
{
	// set font texture
	pDrawList->PushTextureID( pFont->ContainerAtlas->TexID );

	if (uFlags & DRAW_TEXT_DROPSHADOW)
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 1.0f, 1.0f ), colOutline, szText.c_str( ) );
	else if (uFlags & DRAW_TEXT_OUTLINE)
	{
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, -1.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( 0.0f, 1.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2( -1.0f, 0.0f ), colOutline, szText.c_str( ) );
		pDrawList->AddText( pFont, flFontSize, vecPosition + ImVec2(1.0f, 0.0f), colOutline, szText.c_str( ) );
	}

	pDrawList->AddText( pFont, flFontSize, vecPosition, colText, szText.data( ) );
	pDrawList->PopTextureID( );
}
#pragma endregion

#pragma region draw_extra
bool Draw::WorldToScreen( const Vector& vecOrigin, ImVec2& vecScreen )
{
	const ViewMatrix_t& matWorldToScreen = Globals::m_ViewMatrix;
	const float flWidth = matWorldToScreen[ 3 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 3 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 3 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 3 ][ 3 ];

	if (flWidth < 0.001f)
		return false;

	// compute the scene coordinates of a point in 3d
	const float flInverse = 1.0f / flWidth;
	vecScreen.x = ( matWorldToScreen[ 0 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 0 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 0 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 0 ][ 3 ] ) * flInverse;
	vecScreen.y = ( matWorldToScreen[ 1 ][ 0 ] * vecOrigin.x + matWorldToScreen[ 1 ][ 1 ] * vecOrigin.y + matWorldToScreen[ 1 ][ 2 ] * vecOrigin.z + matWorldToScreen[ 1 ][ 3 ] ) * flInverse;

	// screen transform
	// get the screen position in pixels of given point
	const ImVec2 vecDisplaySize = ImGui::GetIO( ).DisplaySize;
	vecScreen.x = ( vecDisplaySize.x * 0.5f ) + ( vecScreen.x * vecDisplaySize.x ) * 0.5f;
	vecScreen.y = ( vecDisplaySize.y * 0.5f ) - ( vecScreen.y * vecDisplaySize.y ) * 0.5f;
	return true;
}

void Draw::Render3DCircleOneteap(Vector vecPosition, Color colColor, int nPointCount, float flRadius, bool bFade, float flRotationStart, float flFadeStart, float flFadeLength)
{
	float flStep = static_cast<float>(M_PI) * 2.0f / nPointCount;
	std::vector<Vector> points3d;

	int iAlpha = 255;
	int iFadeStartPoint = 0;
	int iFadeEndPoint = 0;
	int iFadeStep = 0;
	if (bFade)
	{
		iFadeStartPoint = (int)(nPointCount * flFadeStart);
		iFadeEndPoint = iFadeStartPoint + (int)(nPointCount * flFadeLength);
		iFadeStep = 255 / (iFadeEndPoint - iFadeStartPoint);
	}

	Color colOuter = Color(0, 0, 0, 255);
	Color colInner = colColor;

	for (int i = 0; i < nPointCount; i++)
	{
		if (bFade && i > iFadeEndPoint)
			break;

		float flTheta = (i * flStep) - (static_cast<float>(M_PI) * 2.f * flRotationStart);

		Vector vecWorldStart(flRadius * cosf(flTheta) + vecPosition.x, flRadius * sinf(flTheta) + vecPosition.y, vecPosition.z);
		Vector vecWorldEnd(flRadius * cosf(flTheta + flStep) + vecPosition.x, flRadius * sinf(flTheta + flStep) + vecPosition.y, vecPosition.z);

		ImVec2 vecStart, vecEnd;
		if (!Draw::WorldToScreen(vecWorldStart, vecStart) || !Draw::WorldToScreen(vecWorldEnd, vecEnd))
			return;

		if (bFade && i >= iFadeStartPoint)
		{
			iAlpha -= iFadeStep;

			if (iAlpha < 0)
				iAlpha = 0;
		}

		Draw::AddLine(ImVec2(vecStart.x, vecStart.y + 1), ImVec2(vecEnd.x + 1, vecEnd.y + 1), Color(colOuter.r(), colOuter.g(), colOuter.b(), static_cast<uint8_t>(iAlpha)), 1.f);
		Draw::AddLine(ImVec2(vecStart.x, vecStart.y - 1), ImVec2(vecEnd.x - 1, vecEnd.y - 1), Color(colOuter.r(), colOuter.g(), colOuter.b(), static_cast<uint8_t>(iAlpha)), 1.f);
		Draw::AddLine(ImVec2(vecStart.x, vecStart.y), ImVec2(vecEnd.x, vecEnd.y), Color(colInner.r(), colInner.g(), colInner.b(), static_cast<uint8_t>(iAlpha)), 1.f);;
	}
}

void Draw::CircleGradient(ImDrawList* pDrawList, ImVec2 vecPosition, float flRadius, Color colIn, Color colOut)
{
	if (colIn.a() == 0 && colOut.a() == 0 || flRadius < 0.5f)
		return;

	float flInnerRadiusFactor = 1.2f;

	float flInnerRadius = flRadius * flInnerRadiusFactor;
	if (flInnerRadius < 0.5f)
		flInnerRadius = 0.5f;

	// Draw the inner circle with the inner color
	if (flInnerRadius > 0.0f)
	{
		pDrawList->PathArcToFast(ImVec2(vecPosition.x, vecPosition.y), flInnerRadius, 0, 48);
		pDrawList->PathFillConvex(colIn.GetU32());
	}

	// Draw the outer part with the gradient
	pDrawList->PathArcToFast(ImVec2(vecPosition.x, vecPosition.y), flRadius, 0, 48);
	const int count = pDrawList->_Path.Size - 1;

	unsigned int vtx_base = pDrawList->_VtxCurrentIdx;
	pDrawList->PrimReserve(count * 3, count + 1);

	// Submit vertices
	const ImVec2 uv = pDrawList->_Data->TexUvWhitePixel;
	pDrawList->PrimWriteVtx(ImVec2(vecPosition.x, vecPosition.y), uv, colIn.GetU32());
	for (int n = 0; n < count; n++)
		pDrawList->PrimWriteVtx(pDrawList->_Path[n], uv, colOut.GetU32());

	// Submit a fan of triangles
	for (int n = 0; n < count; n++)
	{
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base));
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % count)));
	}
	pDrawList->_Path.Size = 0;
}

void Draw::CircleRainbow(ImDrawList* pDrawList, ImVec2 vecPosition, float flRadius, bool bRotate)
{
	static float flBaseHue = 0.f;
	if (bRotate)
	{
		if (flBaseHue >= 1.f)
			flBaseHue = 0.f;

		flBaseHue += 0.1f * ImGui::GetIO().DeltaTime;
	}

	// Use arc with automatic segment count
	pDrawList->PathArcToFast(ImVec2(vecPosition.x, vecPosition.y), flRadius, 0, 48);
	const int nCount = pDrawList->_Path.Size - 1;

	unsigned int vtx_base = pDrawList->_VtxCurrentIdx;
	pDrawList->PrimReserve(nCount * 3, nCount + 1);

	const ImVec2 uv = pDrawList->_Data->TexUvWhitePixel;
	pDrawList->PrimWriteVtx(ImVec2(vecPosition.x, vecPosition.y), uv, Color(0, 0, 0, 0).GetU32());

	float flNewHue = flBaseHue;
	for (int n = 0; n < nCount; n++)
	{
		if (flNewHue >= 1.f)
			flNewHue = 0.f;

		flNewHue += (1.0f / nCount);
		flNewHue = std::clamp(flNewHue, 0.f, 1.f);

		Color colTemp = Color::FromHSB(flNewHue, 1.f, 1.f);
		pDrawList->PrimWriteVtx(pDrawList->_Path[n], uv, colTemp.GetU32());
	}

	// Submit a fan of triangles
	for (int n = 0; n < nCount; n++)
	{
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base));
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + n));
		pDrawList->PrimWriteIdx((ImDrawIdx)(vtx_base + 1 + ((n + 1) % nCount)));
	}
	pDrawList->_Path.Size = 0;
}

#pragma endregion

std::vector<std::byte>::iterator FindSignature(std::vector<std::byte>& vecRaw, const std::vector<std::byte>& signature) {
	auto it = std::search(vecRaw.begin(), vecRaw.end(), signature.begin(), signature.end());
	return it; // Return iterator to the beginning of the found sequence
}
#pragma region image_creation
// TODO: Weapon, gloves images here
ID3D11ShaderResourceView* Draw::GetWeaponPreview(const char* szWeaponName, const std::string& szSkinName)
{
	//IDirect3DTexture9* skin_image = nullptr;
	//std::string vpk_path;

	//if (strcmp(szWeaponName, XorStr("unknown")) && strcmp(szWeaponName, XorStr("knife")) && strcmp(szWeaponName, XorStr("gloves"))) //-V526
	//{
	//	if (szSkinName.empty() || szSkinName == XorStr("default"))
	//		vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/") + std::string(szWeaponName) + XorStr(".png");
	//	else
	//		vpk_path = XorStr("resource/flash/econ/default_generated/") + std::string(szWeaponName) + XorStr("_") + std::string(szSkinName) + XorStr("_light_large.png");
	//}
	//else
	//{
	//	if (!strcmp(szWeaponName, XorStr("knife")))
	//		vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/weapon_knife.png");
	//	else if (!strcmp(szWeaponName, XorStr("gloves")))
	//		vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
	//	else if (!strcmp(szWeaponName, XorStr("unknown")))
	//		vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/weapon_snowball.png");

	//}
	//const auto handle = I::FileSystem->Open(vpk_path.c_str(), XorStr("r"), XorStr("GAME"));
	//if (handle)
	//{
	//	int file_len = I::FileSystem->Size(handle);
	//	char* image = new char[file_len]; //-V121

	//	I::FileSystem->Read(image, file_len, handle);
	//	I::FileSystem->Close(handle);

	//	D3DXCreateTextureFromFileInMemory(pDevice, image, file_len, &skin_image);
	//	delete[] image;
	//}

	//if (!skin_image)
	//{
	//	std::string vpk_path;

	//	//if (strstr(szWeaponName, XorStr("bloodhound")) != NULL || strstr(szWeaponName, XorStr("hydra")) != NULL)
	//		//vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/ct_gloves.png");
	//	//else
	//	vpk_path = XorStr("resource/flash/econ/weapons/base_weapons/") + std::string(szWeaponName) + XorStr(".png");

	//	const auto handle = I::FileSystem->Open(vpk_path.c_str(), XorStr("r"), XorStr("GAME"));

	//	if (handle)
	//	{
	//		int file_len = I::FileSystem->Size(handle);
	//		char* image = new char[file_len]; //-V121

	//		I::FileSystem->Read(image, file_len, handle);
	//		I::FileSystem->Close(handle);

	//		D3DXCreateTextureFromFileInMemory(pDevice, image, file_len, &skin_image);
	//		delete[] image;
	//	}
	//}

	ID3D11ShaderResourceView* pSkinImage = nullptr;
	//std::string strVTexPath = X("panorama/images/econ/default_generated/weapon_deagle_am_tiger_silver_light_large_png.vtex_c");

	//int nFileSize = 0;
	//void* pData = Utilities::LoadFileForMe(strVTexPath.c_str(), nFileSize);

	//static std::vector<std::byte> PngSignature = 
	//{
	//	std::byte(0x89), std::byte(0x50), std::byte(0x4E), std::byte(0x47),std::byte(0x0D), std::byte(0x0A), std::byte(0x1A)
	//};

	//std::vector<std::byte> vecRaw;
	//vecRaw.resize(nFileSize);

	//CRT::MemoryCopy(vecRaw.data(), pData, nFileSize);
	//
	//// Find the position of the PngSignature in vecRaw
	//auto it = FindSignature(vecRaw, PngSignature);

	//if (it != vecRaw.end()) 
	//{
	//	std::cout << "PngSignature found at index: " << std::distance(vecRaw.begin(), it) << std::endl;
	//}
	//else 
	//{
	//	std::cout << "PngSignature not found in vecRaw." << std::endl;
	//	std::cout << "vecRaw size: " << vecRaw.size() << std::endl;
	//	std::cout << "PngSignature size: " << PngSignature.size() << std::endl;

	//	std::cout << "Contents of vecRaw:" << std::endl;
	//	for (const auto& byte : vecRaw) {
	//		std::cout << std::hex << static_cast<int>(byte) << " ";
	//	}
	//	std::cout << std::endl;
	//}


	//uint8_t* pImage = reinterpret_cast<uint8_t*>(&*it);
	//SIZE_T ImageSize = vecRaw.end() - it;

	//int iWidth, iHeight;
	//Helpers::LoadTextureFromMemory(pImage, ImageSize, &pSkinImage, &iWidth, &iHeight);

	return pSkinImage;
}
#pragma endregion