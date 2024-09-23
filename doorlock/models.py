from django.contrib.auth.models import AbstractUser
from django.db import models

# Create your models here.
class CustomUser(AbstractUser):
    email = models.EmailField(unique=True)  # Ensure email uniqueness
    profile_picture = models.URLField(null=True, blank=True)  # Allow for no profile picture
    
    def __str__(self):
        return self.email