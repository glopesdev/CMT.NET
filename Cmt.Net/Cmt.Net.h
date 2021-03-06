#pragma once

#include <CMT.h>
#include "opencv2/core/core.hpp"
using namespace System;
using namespace System::Collections;
using namespace System::Collections::Generic;

namespace Cmt
{
	namespace Net
	{
		/// <summary>
		/// Represents the consensus based matching and tracking algorithm.
		/// </summary>
		public ref class TrackerCmt
		{
		private:
			cmt::CMT *cmt;
			OpenCV::Net::Arr ^previous;
			~TrackerCmt() { this->!TrackerCmt(); }
			!TrackerCmt();
		public:
			/// <summary>
			/// Initializes a new instance of the <see cref="TrackerCmt"/> class.
			/// </summary>
			TrackerCmt();

			/// <summary>
			/// Initializes the tracker with the given image and bounding box.
			/// </summary>
			/// <param name="image">The source image representing the initial state of tracking.</param>
			/// <param name="rect">The bounding box representing the initial state of tracking.</param>
			void Initialize(OpenCV::Net::Arr ^image, OpenCV::Net::Rect rect);

			/// <summary>
			/// Updates the state of the tracker with a new image.
			/// </summary>
			/// <param name="image">The source image on which to look for the target.</param>
			void ProcessFrame(OpenCV::Net::Arr ^image);

			/// <summary>
			/// Gets or sets a value indicating whether to estimate the scale of the tracked object.
			/// </summary>
			property bool EstimateScale
			{
				bool get() { return cmt->consensus.estimate_scale; }
				void set(bool value) { cmt->consensus.estimate_scale = value; }
			}

			/// <summary>
			/// Gets or sets a value indicating whether to estimate the rotation of the tracked object.
			/// </summary>
			property bool EstimateRotation
			{
				bool get() { return cmt->consensus.estimate_rotation; }
				void set(bool value) { cmt->consensus.estimate_rotation = value; }
			}

			/// <summary>
			/// Gets the currently tracked bounding box.
			/// </summary>
			property OpenCV::Net::RotatedRect BoundingBox
			{
				OpenCV::Net::RotatedRect get()
				{
					return OpenCV::Net::RotatedRect(
						OpenCV::Net::Point2f(cmt->bb_rot.center.x, cmt->bb_rot.center.y),
						OpenCV::Net::Size2f(cmt->bb_rot.size.width, cmt->bb_rot.size.height),
						cmt->bb_rot.angle);
				}
			}

			/// <summary>
			/// Gets the confidence level for the currently tracked bounding box.
			/// </summary>
			property float Confidence
			{
				float get() { return cmt->confidence; }
			}

			/// <summary>
			/// Gets the currently active points for visualization purposes.
			/// </summary>
			property System::Collections::Generic::IEnumerable<OpenCV::Net::Point2f> ^ ActivePoints
			{
				System::Collections::Generic::IEnumerable<OpenCV::Net::Point2f> ^ get()
				{
					return gcnew ActivePointEnumerable(cmt);
				}
			}

		private:
			ref class ActivePointEnumerable : public System::Collections::Generic::IEnumerable<OpenCV::Net::Point2f>
			{
			private:
				cmt::CMT *cmt;
			internal:
				ActivePointEnumerable(cmt::CMT *cmt):
					cmt(cmt)
				{
				}

			public:
				virtual System::Collections::IEnumerator ^ EnumerableGetEnumerator() =
					System::Collections::IEnumerable::GetEnumerator
				{
					return this->GenericEnumerableGetEnumerator();
				}

				virtual System::Collections::Generic::IEnumerator<OpenCV::Net::Point2f> ^ GenericEnumerableGetEnumerator() =
					System::Collections::Generic::IEnumerable<OpenCV::Net::Point2f>::GetEnumerator
				{
					return gcnew ActivePointEnumerator(cmt);
				}
			};

			ref class ActivePointEnumerator : public System::Collections::Generic::IEnumerator<OpenCV::Net::Point2f>
			{
			private:
				int index;
				cmt::CMT *cmt;

			internal:
				ActivePointEnumerator(cmt::CMT *cmt):
					cmt(cmt),
					index(-1)
				{
				}

				property Object ^ CurrentNonGeneric {
					virtual Object ^ get() = System::Collections::IEnumerator::Current::get { return Current; }
				}

			public:
				~ActivePointEnumerator() { }
				virtual void Reset() { index = -1; }

				property OpenCV::Net::Point2f Current {
					virtual OpenCV::Net::Point2f get() { return *((OpenCV::Net::Point2f*)&cmt->points_active.at(index)); }
				}

				virtual bool MoveNext()
				{
					return ++index < cmt->points_active.size();
				}
			};
		};
	}
}

