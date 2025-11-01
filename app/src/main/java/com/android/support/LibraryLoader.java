package com.android.support;

import android.content.Context;
import android.content.res.AssetManager;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import android.widget.Toast;
import java.io.IOException;

public class LibraryLoader {

    public static void loadLibraryFromAssets(Context context, String assetName, String libName) throws IOException {
        AssetManager assetManager = context.getAssets();
        
        try (InputStream inputStream = assetManager.open(assetName)) {
            File tempFile = File.createTempFile(libName, null, context.getCacheDir());
            
            tempFile.deleteOnExit();
            
            try (FileOutputStream outputStream = new FileOutputStream(tempFile)) {
                byte[] buffer = new byte[4096];
                int bytesRead;
                while ((bytesRead = inputStream.read(buffer)) != -1) {
                    outputStream.write(buffer, 0, bytesRead);
                }
            }
//            Toast.makeText(context, tempFile.getAbsolutePath(), Toast.LENGTH_LONG).show();
            
            System.load(tempFile.getAbsolutePath());
        }
    }
}