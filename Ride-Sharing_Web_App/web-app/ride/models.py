from django.db import models
# from django.contrib.auth.models import User
from user.models import UberUser, VEHICLE_TYPE_CHOICES
# Create your models here.
from django.core.validators import MaxValueValidator, MinValueValidator

OWNER = 'O'
SHARER = 'S'

CT_TYPE_CHOICES = {
    (OWNER, 'Owner'),
    (SHARER, 'Sharer')
}

class Ride(models.Model):
    id = models.AutoField(primary_key=True)
    owner = models.ForeignKey(UberUser, on_delete=models.CASCADE, related_name='ride_owner')
    driver = models.ForeignKey(UberUser, on_delete=models.CASCADE,related_name='ride_driver',null=True,blank = True)
    sharer = models.ManyToManyField(UberUser,related_name='ride_sharer', blank=True)

    from_location = models.TextField(blank=False)
    dest_location = models.TextField(blank=False)
    arr_date_time = models.DateTimeField(blank=False)
    max_passenger = models.IntegerField(default = 1, validators=[
            MaxValueValidator(20),
            MinValueValidator(1)
        ])

    vehicle_type = models.CharField(max_length=2, choices=VEHICLE_TYPE_CHOICES, blank=True, null=True)
    special_vehicle_info = models.TextField(blank=True, null=True)
    license_plate_number = models.CharField(max_length=8, blank=True, default='')

    # TODO: add shared
    share_flag = models.BooleanField(default=False, blank=False)
    is_confirm = models.BooleanField(default=False, blank=False)
    is_complete = models.BooleanField(default=False, blank=False)

class Party(models.Model):
    user = models.ForeignKey(UberUser, on_delete=models.CASCADE, related_name='party_owner')
    num_passenger = models.IntegerField(default=1, validators=[
            MaxValueValidator(20),
            MinValueValidator(1)
        ])
    ride = models.ForeignKey(Ride, on_delete=models.CASCADE, related_name='ride_party')
    joined_as = models.CharField(max_length=1, choices=CT_TYPE_CHOICES, blank=True, null=True)
    start_time = models.DateTimeField(blank=True, null=True)
    end_time = models.DateTimeField(blank=True, null=True)
