#pragma once

#include "Object.h"
#include "WICDecoder.h"
#include "BaseTypes.h"

#include <d2d1.h>

#include <string>

namespace ChaoticLib{ namespace Direct2D{

	class Image: public Object{
	public:

#ifndef DONT_USE_ENUM_CLASS
		enum class DrawingMode: int{
			Stretch,
			Squeeze,
			Trim,
		};
#else
		struct DrawingMode{
			static const int Stretch = 0;
			static const int Squeeze = 1;
			static const int Trim = 2;

			int mode;

			DrawingMode(int m): mode(m)
			{
			}
			operator int()
			{
				return mode;
			}
		};
#endif
#ifndef DONT_USE_ENUM_CLASS
		enum class InterpolationMode: unsigned{
			NearestNeighbor = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR,
			Linear = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
		};
#else
		struct InterpolationMode{
			static const D2D1_BITMAP_INTERPOLATION_MODE NearestNeighbor = D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR;
			static const D2D1_BITMAP_INTERPOLATION_MODE Linear = D2D1_BITMAP_INTERPOLATION_MODE_LINEAR;

			D2D1_BITMAP_INTERPOLATION_MODE interpolationmode;

			InterpolationMode(D2D1_BITMAP_INTERPOLATION_MODE im): interpolationmode(im)
			{
			}
			operator D2D1_BITMAP_INTERPOLATION_MODE()
			{
				return interpolationmode;
			}
		};
#endif

	private:
		WICDecoder decoder;
		ID2D1Bitmap *bmp;
		std::wstring filename;
		Rect clippingrect, src, dest;
		DrawingMode drawingmode;
		InterpolationMode interpolationmode;
		bool modified;

		bool Create(ID2D1RenderTarget *);
		void CalcRectangle();

	public:
		Image(const wchar_t *filename=L""):
			bmp(nullptr),
			filename(filename),
			drawingmode(DrawingMode::Stretch),
			interpolationmode(InterpolationMode::Linear),
			modified(true)
		{
		}
		Image(const Image &);
		~Image();

		void SetDrawingMode(DrawingMode);
		void SetInterpolationMode(InterpolationMode);
		void SetFileName(const wchar_t *);
		void SetClippingRect(Rect);
		Rect GetClippingRect() const;

		virtual void SetPosition(const Point &) override;
		virtual void SetSize(const Size &) override;

		virtual bool CreateResource(const CreateStruct &) override;
		virtual void DestroyResource() override;
		virtual void Draw(const PaintStruct &) override;
		ID2D1Bitmap *Get() const;
	};

} }
