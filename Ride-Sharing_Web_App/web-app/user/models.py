# from random import choice
from django.db import models
from django.contrib.auth.models import AbstractUser
# Create your models here.

VEHICLE_EXTRA_LARGE = 'XL'
VEHICLE_LARGE = 'L'
VEHICLE_MED = 'M'
VEHICLE_SMALL = 'S'

VEHICLE_TYPE_CHOICES = {
    (VEHICLE_EXTRA_LARGE, 'XL'),
    (VEHICLE_LARGE, 'L'),
    (VEHICLE_MED, 'M'),
    (VEHICLE_SMALL, 'S')
}

DRIVER = 'DR'
CUSTOMER = 'CT'

USER_TYPE_CHOICES = {
    (DRIVER, 'Driver'),
    (CUSTOMER, 'Customer')
}

class UberUser(AbstractUser):
    # user = models.OneToOneField(User, on_delete=models.CASCADE)
    # primary_key = True
    # username = models.CharField(max_length=15)

    user_type = models.CharField(max_length=8, choices=USER_TYPE_CHOICES, default=CUSTOMER)

    # only required if user is driver

    vehicle_type = models.CharField(max_length=2, choices=VEHICLE_TYPE_CHOICES, blank=True, default='')
    license_plate_number = models.CharField(max_length=8, blank=True, default='')
    max_passenger = models.IntegerField(blank=True, null=True)
    special_vehicle_info = models.TextField(blank=True, null=True)
