#pragma once


#define NP_SLATE_BLOCK_ALL_INPUT() \
	public: \
		virtual bool SupportsKeyboardFocus() const override \
		{ \
			return true; \
		} \
		\
		virtual FReply OnKeyDown(const FGeometry&, const FKeyEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnKeyUp(const FGeometry&, const FKeyEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnAnalogValueChanged(const FGeometry&, const FAnalogInputEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnMouseButtonDown(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnMouseButtonUp(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnMouseButtonDoubleClick(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnMouseMove(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnMouseWheel(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnTouchStarted(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnTouchMoved(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
		\
		virtual FReply OnTouchEnded(const FGeometry&, const FPointerEvent&) override \
		{ \
			return FReply::Handled(); \
		} \
	private: