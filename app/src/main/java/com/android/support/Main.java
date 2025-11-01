package com.android.support;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Handler;
import android.provider.Settings;
import android.widget.Toast;
import java.io.IOException;
import android.content.pm.PackageManager;

import android.app.Activity;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Environment;
import android.provider.Settings;
import android.content.pm.PackageManager;
import android.Manifest;

public class Main {

    public static void loadLibrary(Context context) {
        try {
            LibraryLoader.loadLibraryFromAssets(context, "libBGNX.ss", "libBGNX.ss");
        } catch (IOException e) {
            Toast.makeText(context, "Failed to load lib", Toast.LENGTH_LONG).show();
        }
    }

    private static native void CheckOverlayPermission(Context context);

    public static void StartWithoutPermission(Context context) {
        loadLibrary(context);
        CrashHandler.init(context, true);
        if (context instanceof Activity) {
            Activity activity = (Activity) context;
            PermSender.SendStorage(activity);
            PermSender.SendWindow(activity);
        }
        if (context instanceof Activity) {
            //Check if context is an Activity.
            Menu menu = new Menu(context);
            menu.SetWindowManagerActivity();
            menu.ShowMenu();
        } else {
            Toast.makeText(context, "Failed to launch the mod menu\n", Toast.LENGTH_LONG).show();
        }
    }

    public static void Start(Context context) {
        //System.loadLibrary("BMGX");
		loadLibrary(context);
        CrashHandler.init(context, false);

        CheckOverlayPermission(context);
    }
}
