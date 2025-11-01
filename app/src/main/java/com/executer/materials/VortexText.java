package com.executer.materials;

import android.content.Context;
import android.graphics.Color;
import android.graphics.Typeface;
import android.graphics.Paint;
import android.graphics.drawable.GradientDrawable;
import android.os.Build;
import android.os.Handler;
import android.text.TextUtils;
import android.view.Gravity;
import android.view.WindowManager;
import android.widget.FrameLayout;
import android.widget.TextView;

public class VortexText {
    private static Context appContext;
    private static VortexText instance;

    private final WindowManager windowManager;
    private final FrameLayout layout;
    private final TextView textView;
    private final GradientDrawable backgroundDrawable;
    private final Handler handler = new Handler();
    private int colorIndex = 0;
    private float colorProgress = 0f;
    private final int[] colors = {Color.RED, Color.BLACK, Color.WHITE, Color.GREEN};

    private VortexText(Context context, String msg) {
        appContext = context.getApplicationContext();
        windowManager = (WindowManager) appContext.getSystemService(Context.WINDOW_SERVICE);

        layout = new FrameLayout(appContext);

        // Semi-transparent rounded background
        backgroundDrawable = new GradientDrawable();
        backgroundDrawable.setColor(Color.parseColor("#44FFFFFF")); // subtle transparency
        backgroundDrawable.setCornerRadius(24f);

        textView = new TextView(appContext);
        textView.setText(msg);
        textView.setTextSize(18f);
        textView.setSingleLine(true); // prevent wrapping
        textView.setGravity(Gravity.CENTER);
        textView.setTypeface(Typeface.DEFAULT_BOLD, Typeface.BOLD);
        textView.setBackground(backgroundDrawable);
        textView.setClickable(false);
        textView.setFocusable(false);

        layout.addView(textView);

        // Set layout params based on text width
        textView.measure(0, 0);
        FrameLayout.LayoutParams params = new FrameLayout.LayoutParams(
                textView.getMeasuredWidth() + 20, // small padding around text
                textView.getMeasuredHeight() + 12
        );
        params.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
        params.topMargin = 122; // 2px lower than notch
        layout.setLayoutParams(params);

        startColorAnimation();
    }

    private void startColorAnimation() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                if (textView != null) {
                    // Smooth color animation between Red → Black → White → Green
                    int c1 = colors[(int) colorProgress % colors.length];
                    int c2 = colors[((int) colorProgress + 1) % colors.length];
                    float fraction = colorProgress - (int) colorProgress;
                    int r = (int) (Color.red(c1) + fraction * (Color.red(c2) - Color.red(c1)));
                    int g = (int) (Color.green(c1) + fraction * (Color.green(c2) - Color.green(c1)));
                    int b = (int) (Color.blue(c1) + fraction * (Color.blue(c2) - Color.blue(c1)));
                    textView.setTextColor(Color.rgb(r, g, b));

                    colorProgress += 0.02f;
                    if (colorProgress >= colors.length) colorProgress = 0f;
                }
                handler.postDelayed(this, 30);
            }
        });
    }

    /** Initialize overlay */
    public static void init(Context context) {
        appContext = context.getApplicationContext();
    }

    /** Show overlay text */
    public static void showText(String msg) {
        if (appContext == null) throw new IllegalStateException("VortexText not initialized!");
        if (instance != null) return;

        instance = new VortexText(appContext, msg);

        int type = Build.VERSION.SDK_INT >= Build.VERSION_CODES.O
                ? WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY
                : WindowManager.LayoutParams.TYPE_PHONE;

        WindowManager.LayoutParams wmParams = new WindowManager.LayoutParams(
                instance.textView.getMeasuredWidth() + 20,
                instance.textView.getMeasuredHeight() + 12,
                type,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                        | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE
                        | WindowManager.LayoutParams.FLAG_LAYOUT_NO_LIMITS
                        | WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN,
                android.graphics.PixelFormat.TRANSLUCENT
        );
        wmParams.gravity = Gravity.TOP | Gravity.CENTER_HORIZONTAL;
        wmParams.y = 122; // slightly below notch

        try {
            instance.windowManager.addView(instance.layout, wmParams);
        } catch (Exception ignored) {}
    }

    /** Remove overlay text */
    public static void removeText() {
        if (instance == null) return;
        try {
            instance.windowManager.removeView(instance.layout);
        } catch (Exception ignored) {}
        instance = null;
    }

    /** Update text dynamically */
    public static void updateText(String msg) {
        if (instance != null && instance.textView != null) {
            instance.textView.setText(msg);
            instance.textView.measure(0, 0);
            // Update overlay size
            if (instance.layout.getLayoutParams() instanceof WindowManager.LayoutParams) {
                WindowManager.LayoutParams lp = (WindowManager.LayoutParams) instance.layout.getLayoutParams();
                lp.width = instance.textView.getMeasuredWidth() + 20;
                lp.height = instance.textView.getMeasuredHeight() + 12;
                instance.windowManager.updateViewLayout(instance.layout, lp);
            }
        }
    }
}