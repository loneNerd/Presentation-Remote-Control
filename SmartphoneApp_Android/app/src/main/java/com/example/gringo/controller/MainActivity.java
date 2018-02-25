package com.example.gringo.controller;

import android.os.AsyncTask;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

import java.io.DataOutputStream;
import java.io.IOException;
import java.lang.String;
import java.net.Socket;

public class MainActivity extends AppCompatActivity {

    String serIpAddress = "";
    final int port = 10000;
    final byte test = 0; //Test command
    final byte nextSlide = 1;
    final byte prevSlide = 2;
    boolean successIP = false; //Validation of IP
    byte codeCommand = 0; //Variable for buttons indexes

    //Alpha animation for button
    Animation animAlpha;// = AnimationUtils.loadAnimation(this, R.anim.alpha);

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        animAlpha = AnimationUtils.loadAnimation(this, R.anim.alpha);
    }

    //That method is writes IP
    public void SaveIP(View v) {
        //Some animation for button
        v.startAnimation(animAlpha);

        EditText etIpAddress = findViewById(R.id.edIpAddress);
        serIpAddress = etIpAddress.getText().toString();

        if (!serIpAddress.isEmpty())
        {
            //Checking IP Validation
            codeCommand = test;
            SenderThread sender = new SenderThread();
            sender.execute();

            try {
                Thread.sleep(50);
            } catch (Exception ex) {}

            if (successIP)
                this.findViewById(R.id.IPAddressLine).setVisibility(View.INVISIBLE);
            else
            {
                Toast.makeText(this, "Enter correct server IP to continue", Toast.LENGTH_SHORT).show();
                serIpAddress = "";
            }
        }
        else //If Ip address line Empty
            Toast.makeText(this, "Enter server IP to continue", Toast.LENGTH_SHORT).show();

    }

    public void onClick(View v) {
        //Some animation for buttons
        v.startAnimation(animAlpha);

        //Checking IP address line
        if (serIpAddress.isEmpty()) {
            Toast.makeText(this, "Enter server IP to continue", Toast.LENGTH_SHORT).show();
            return;
        }

        SenderThread sender = new SenderThread();

        if (v.getId() == R.id.nextSlide)
        {
            codeCommand = nextSlide;
            sender.execute();
        }
        else if (v.getId() == R.id.prevSlide)
        {
            codeCommand = prevSlide;
            sender.execute();
        }

        try {
            Thread.sleep(50);
        } catch (Exception ex) {}

        //Checking for lost communication with server
        if (!successIP) {
            Toast.makeText(this, "Connection Lost", Toast.LENGTH_SHORT).show();
            findViewById(R.id.IPAddressLine).setVisibility(View.VISIBLE);
        }
    }

    class SenderThread extends AsyncTask<Void, Void, Void> {
        @Override
        protected Void doInBackground(Void... params) {
            try {
                //I had written this way 'cause I don't know how write this another way
                //I mean next 3 lines
                successIP = false;
                Socket socket = new Socket(serIpAddress, port);
                successIP = true;
                DataOutputStream out = new DataOutputStream(socket.getOutputStream());
                if (codeCommand == nextSlide)
                    out.write(nextSlide);
                else if (codeCommand == prevSlide)
                    out.write(prevSlide);
                else if (codeCommand == test)
                {
                    out.write(test);
                    successIP = true;
                }
            } catch (IOException ex) {
                ex.getStackTrace();
            }
            return null;
        }
    }
}