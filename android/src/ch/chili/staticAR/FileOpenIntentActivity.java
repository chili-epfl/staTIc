package ch.chili.staticAR;

import android.content.Intent;

import org.qtproject.qt5.android.bindings.QtActivity;
import android.util.Log;
import android.os.Bundle;
import android.net.Uri;

import java.io.FileOutputStream;
import android.content.ContentResolver;
import java.io.InputStream;
import java.io.IOException;
import java.io.File;

public class FileOpenIntentActivity extends QtActivity {
    public void onNewIntent(Intent i) {
        super.onNewIntent(i);
        if (i.getAction() == Intent.ACTION_VIEW) {
            Uri data=i.getData();
            String path=data.getPath();
            String filename=new String();
            if(data.getScheme().equals("content")){
                String[] parts = path.split(":");
                filename=parts[parts.length-1];
            }
            else if(data.getScheme().equals("file")){
                String[] parts = path.split("/");
                filename=parts[parts.length-1];
            }
            File folder = new File(getFilesDir().toString().concat("/temp"));
            boolean success = true;
            //Here we copy the files that will be handled in main.cpp
            if (!folder.exists()) {
                success = folder.mkdirs();
            }
            if (success) {
                if(!filename.isEmpty()){
                    copyFile(data,getFilesDir().toString().concat("/temp/").concat(filename));
                }
            } else {
            }
        }
    }
    public void onCreate(Bundle savedState)
    {
        super.onCreate(savedState);
        onNewIntent(getIntent());
    }

    private void copyFile(Uri src,String dst){
        try{
        InputStream in = getContentResolver().openInputStream(src);
        FileOutputStream out = new FileOutputStream(dst);
        // Transfer bytes from in to out
        byte[] buf = new byte[1024];
        int len;
        while ((len = in.read(buf)) > 0) {
              out.write(buf, 0, len);
        }
        }
        catch (IOException e) {}

    }

}
