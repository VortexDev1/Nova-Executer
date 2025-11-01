package com.executer.materials;

import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

public class CustomDialog {

    private AlertDialog dialog;
    private Context context;

    private String title;
    private String message;

    private String leftText;
    private String middleText;
    private String rightText;

    private OnDialogButtonClickListener leftListener;
    private OnDialogButtonClickListener middleListener;
    private OnDialogButtonClickListener rightListener;

    public interface OnDialogButtonClickListener {
        void onClick();
    }

    public CustomDialog(Context context) {
        this.context = context;
    }

    public CustomDialog setTitle(String title) {
        this.title = title;
        return this;
    }

    public CustomDialog setMessage(String message) {
        this.message = message;
        return this;
    }

    public CustomDialog setLeftButton(String text, OnDialogButtonClickListener listener) {
        this.leftText = text;
        this.leftListener = listener;
        return this;
    }

    public CustomDialog setMiddleButton(String text, OnDialogButtonClickListener listener) {
        this.middleText = text;
        this.middleListener = listener;
        return this;
    }

    public CustomDialog setRightButton(String text, OnDialogButtonClickListener listener) {
        this.rightText = text;
        this.rightListener = listener;
        return this;
    }

    public void show() {
        AlertDialog.Builder builder = new AlertDialog.Builder(context);
        if (title != null) builder.setTitle(title);
        if (message != null) builder.setMessage(message);

        if (leftText != null) {
            builder.setNeutralButton(leftText, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    if (leftListener != null) leftListener.onClick();
                }
            });
        }

        if (middleText != null) {
            builder.setNegativeButton(middleText, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    if (middleListener != null) middleListener.onClick();
                }
            });
        }

        if (rightText != null) {
            builder.setPositiveButton(rightText, new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialogInterface, int i) {
                    if (rightListener != null) rightListener.onClick();
                }
            });
        }

        dialog = builder.create();

        Window window = dialog.getWindow();
        if (window != null) {
            GradientDrawable bg = new GradientDrawable();
            bg.setColor(Color.parseColor("#1B1B1B"));
            bg.setCornerRadius(40f);
            bg.setStroke(4, Color.parseColor("#BB86FC"));
            window.setBackgroundDrawable(bg);

            // Make dialog slightly bigger (90% width of screen)
            int width = (int) (context.getResources().getDisplayMetrics().widthPixels * 0.9);
            window.setLayout(width, ViewGroup.LayoutParams.WRAP_CONTENT);

            if (!(context instanceof android.app.Activity) && Build.VERSION.SDK_INT >= 26) {
                window.setType(WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY);
            }
        }

        dialog.show();

        // Style buttons
        Button neutral = dialog.getButton(AlertDialog.BUTTON_NEUTRAL); // left
        Button negative = dialog.getButton(AlertDialog.BUTTON_NEGATIVE); // middle
        Button positive = dialog.getButton(AlertDialog.BUTTON_POSITIVE); // right

        if (neutral != null) {
            neutral.setTextColor(Color.parseColor("#03DAC5"));
            ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) neutral.getLayoutParams();
            params.rightMargin = 60; // more space between left and others
            neutral.setLayoutParams(params);
        }
        if (negative != null) negative.setTextColor(Color.parseColor("#FF5252"));
        if (positive != null) positive.setTextColor(Color.parseColor("#BB86FC"));
    }

    public void dismiss() {
        if (dialog != null && dialog.isShowing()) dialog.dismiss();
    }
}