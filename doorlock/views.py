import os
from google.auth.transport import requests
from google.oauth2 import id_token
from django.views.decorators.csrf import csrf_exempt
from django.utils.decorators import method_decorator
from django.shortcuts import render, redirect
from django.http import HttpResponse, HttpRequest
import paho.mqtt.publish as publish
from django.contrib.auth.decorators import login_required
from .models import CustomUser

# Create your views here.
def sign_in(request):
    return render(request, 'doorlocks/home.html')


@csrf_exempt
def auth_receiver(request):
    """
    Google calls this URL after the user has signed in with their Google account.
    """
    token = request.POST['credential']

    try:
        user_data =  id_token.verify_oauth2_token(
            token, requests.Request(), os.environ['GOOGLE_OAUTH_CLIENT_ID']
        )
    except ValueError:
        return HttpResponse(status=403)
    
    email = user_data['email']
    user, created = CustomUser.objects.get_or_create(
        email=email, 
        defaults={
            'username': email,
            'profile_picture': user_data['picture'],
                }
        )
    if not created:
        user.profile_picture = user_data['picture']
        user.save()
        
    request.session['user_data'] = user_data

    return redirect('unlock_door')

def sign_out(request):
    del request.session['user_data']
    return redirect('sign_in')

def unlock_door(request):
    mqtt_host = "192.168.0.100"  # MQTT Broker IP
    mqtt_topic = "SC252/doorLock"
    mqtt_message = "unlock"

    publish.single(mqtt_topic, mqtt_message, hostname=mqtt_host)
    return redirect('index')
