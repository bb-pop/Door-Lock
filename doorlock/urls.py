from django.urls import path
from doorlock import views


urlpatterns = [
    path('', views.sign_in, name='sign_in'),
    path('unlock/', views.unlock_door, name='unlock_door'),
    path('sign-out', views.sign_out, name='sign_out'),
    path('auth-receiver', views.auth_receiver, name='auth_receiver'),
]

