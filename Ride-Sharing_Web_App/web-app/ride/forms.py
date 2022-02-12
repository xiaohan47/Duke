from django import forms
from django.contrib.auth.models import User
from user.models import UberUser, VEHICLE_TYPE_CHOICES
from .models import Ride
from django.core.validators import MaxValueValidator, MinValueValidator

class DateInput(forms.DateTimeInput):
    input_type = 'datetime-local'

class rideRequestForm(forms.ModelForm):
    # arr_date_time = forms.DateTimeField(required=True, widget=forms.DateTimeInput(format='%d/%m/%Y %H:%M'))
    class Meta:
        model = Ride
        fields = ['from_location','dest_location','arr_date_time','max_passenger','vehicle_type','special_vehicle_info',
                  'share_flag']
        widgets = {
            'arr_date_time': DateInput()
        }
    labels = {
        'from_location': 'From',
        'dest_location': 'Destination',
        'arr_date_time': 'Your estimated arrival date and time',
        'max_passenger': 'Total number of passengers',
        'vehicle_type': 'Vehicle type',
        'special_vehicle_type': 'Special requests (Special vehicle info)',
        'share_flag': 'Do you want this ride to be shared'
    }


class sharerSearchForm(forms.Form):
    dest_location = forms.CharField(required=True, label='Destination')
    start_time = forms.DateTimeField(required=True, widget=DateInput(), label='Starting time for your search')
    end_time = forms.DateTimeField(required=True, widget=DateInput(), label='Ending time for your search')
    max_passenger = forms.IntegerField(required=True, validators=[
            MaxValueValidator(20),
            MinValueValidator(1)
        ],  label='Number of passengers')


class sharerRequestForm(forms.Form):
    start_time = forms.DateTimeField(required=True, widget=DateInput(), label='Confirm starting time')
    end_time = forms.DateTimeField(required=True, widget=DateInput(), label='Confirm ending time')
    num_passenger = forms.IntegerField(required=True, validators=[
            MaxValueValidator(20),
            MinValueValidator(1)
        ], label='Confirm number of passengers')

    labels = {
        'start_time': 'Confirm starting time',
        'end_time': 'Confirm ending time',
        'max_passenger': 'Confirm number of passengers',
    }


