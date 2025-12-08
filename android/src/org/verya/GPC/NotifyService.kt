package org.verya.GPC

import android.app.*
import android.content.Intent
import android.os.IBinder
import androidx.core.app.NotificationCompat

class NotifyService : Service() {

    companion object {
        var lastTitle: String = "GP Control Running"
        var lastText: String = "Waiting for data..."
        const val CHANNEL_ID = "GPCChannel"
        const val NOTIF_ID = 1

        @JvmStatic
        fun updateNotificationData(title: String, text: String) {
            lastTitle = title
            lastText = text
        }
    }

    override fun onCreate() {
        super.onCreate()
        createChannel()

        val mainIntent = Intent(this, org.qtproject.qt.android.bindings.QtActivity::class.java)
        mainIntent.flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        val pIntent = PendingIntent.getActivity(
            this,
            0,
            mainIntent,
            PendingIntent.FLAG_IMMUTABLE
        )

        val notif = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(lastTitle)
            .setContentText(lastText)
            .setSmallIcon(android.R.drawable.ic_popup_sync)
            .setContentIntent(pIntent)     // ←← مشکل اینجا بود
            .build()

        startForeground(NOTIF_ID, notif)
    }   

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        pushUpdate()
        return START_STICKY
    }

    private fun pushUpdate() {
        val nm = getSystemService(NOTIFICATION_SERVICE) as NotificationManager

        val mainIntent = Intent(this, org.qtproject.qt.android.bindings.QtActivity::class.java)
        val pIntent = PendingIntent.getActivity(
            this,
            0,
            mainIntent,
            PendingIntent.FLAG_IMMUTABLE
        )

        val notif = NotificationCompat.Builder(this, CHANNEL_ID)
            .setContentTitle(lastTitle)
            .setContentText(lastText)
            .setSmallIcon(android.R.drawable.ic_popup_sync)
            .setContentIntent(pIntent)  // ضروری!
            .build()

        nm.notify(NOTIF_ID, notif)
    }

    override fun onBind(intent: Intent?): IBinder? = null

    private fun createChannel() {
        val channel = NotificationChannel(
            CHANNEL_ID,
            "GPC Notification",
            NotificationManager.IMPORTANCE_LOW
        )
        val nm = getSystemService(NOTIFICATION_SERVICE) as NotificationManager
        nm.createNotificationChannel(channel)
    }
}
