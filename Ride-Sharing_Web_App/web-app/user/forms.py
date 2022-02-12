from django import forms
# from django.contrib.auth.models import User

from django.contrib.auth.forms import UserCreationForm
from .models import UberUser, VEHICLE_TYPE_CHOICES

class UserRegisterForm(UserCreationForm):
    email = forms.EmailField(max_length = 254, required=True)
    class Meta:
        model = UberUser
        fields = ['username','email','password1','password2']


class DriverRegisterForm(forms.Form):
    first_name = forms.CharField(max_length=150, required=True)
    last_name = forms.CharField(max_length=150, required=True)
    vehicle_type = forms.ChoiceField(choices=VEHICLE_TYPE_CHOICES, required=True)
    license_plate_number = forms.CharField(max_length=8, required=True)
    max_passenger = forms.IntegerField(required=True)
    special_vehicle_info = forms.CharField(required=False)

class UserUpdateForm(forms.ModelForm):
    email = forms.EmailField(max_length = 254, required =True)
    class Meta:
        model = UberUser
        fields = ['email']

class DriverUpdateForm(forms.ModelForm):
    class Meta:
        model = UberUser
        fields = ['first_name','last_name','vehicle_type','license_plate_number','max_passenger','special_vehicle_info']