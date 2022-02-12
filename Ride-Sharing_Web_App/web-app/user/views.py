from django.shortcuts import get_object_or_404, render,redirect
from django import forms
from django.views.generic import View
from django.contrib.auth import login
from django.contrib.auth.forms import UserCreationForm
from django.contrib import messages
from .models import UberUser
from .forms import UserRegisterForm, DriverRegisterForm,UserUpdateForm, DriverUpdateForm

# Register for new user, first time will only register for customer
def register(request):
    if request.method == 'POST':
        form = UserRegisterForm(request.POST)
        if form.is_valid():
            user = form.save()
            messages.success(request, 'You have succesfully registered and can now login')
            return redirect('login')
    else:
        form = UserRegisterForm()
    return render(request,'user/register.html',{'form':form})

# Driver register if needed
def driverRegister(request):
    if request.method == "POST":
        form = DriverRegisterForm(request.POST)
        if form.is_valid():
            first_name = form.cleaned_data['first_name']
            last_name = form.cleaned_data['last_name']
            vehicle_type = form.cleaned_data['vehicle_type']
            license_plate_number = form.cleaned_data['license_plate_number']
            max_passenger = form.cleaned_data['max_passenger']
            special_vehicle_info = form.cleaned_data['special_vehicle_info']


            uber_user = UberUser.objects.get(username=request.user.username)
            uber_user.first_name = first_name
            uber_user.last_name = last_name
            uber_user.vehicle_type = vehicle_type
            uber_user.license_plate_number = license_plate_number
            uber_user.max_passenger = max_passenger
            uber_user.special_vehicle_info = special_vehicle_info
            uber_user.save()
            return redirect('home')
    else:
        form = DriverRegisterForm()
    return render(request,'user/driverRegister.html',{'form':form})


# Switch role between customer ( owner, sharer) and driver.
def roleSwitch(request):
    uber_user = get_object_or_404(UberUser, pk=request.user.id)
    if uber_user.user_type == 'CT':
        if uber_user.vehicle_type == '':
            messages.info(request,'Please fill in the information to register as driver')
            return redirect('driver-register')
        else:
            messages.success(request, 'Switched to driver')
            uber_user.user_type = 'DR'
    else:
        messages.success(request, 'Switched to normal user')
        uber_user.user_type = 'CT'
    uber_user.save()
    return redirect('home')

# Show Profile information with two form: customer and driver
def profile(request):
    u_form = UserUpdateForm()
    d_form = DriverUpdateForm()
    context = {
        'u_form': u_form,
        'd_form': d_form
    }
    return render(request, 'user/profile.html', context)

# Customer profile update
def customerUpdate(request):
    if request.method == "POST":
        form = UserUpdateForm(request.POST)
        if form.is_valid():
            email = form.cleaned_data['email']
            uber_user = UberUser.objects.get(username=request.user.username)
            uber_user.email = email
            uber_user.save()
            return redirect('profile')
    else:
        form = UserUpdateForm()
    return render(request,'user/ctUpdate.html',{'form':form})

# Driver profile update
def driverUpdate(request):
    if request.method == "POST":
        form = DriverUpdateForm(request.POST)
        if form.is_valid():
            first_name = form.cleaned_data['first_name']
            last_name = form.cleaned_data['last_name']
            vehicle_type = form.cleaned_data['vehicle_type']
            license_plate_number = form.cleaned_data['license_plate_number']
            max_passenger = form.cleaned_data['max_passenger']
            special_vehicle_info = form.cleaned_data['special_vehicle_info']
            uber_user = UberUser.objects.get(username=request.user.username)
            uber_user.first_name = first_name
            uber_user.last_name = last_name
            uber_user.vehicle_type = vehicle_type
            uber_user.license_plate_number = license_plate_number
            uber_user.max_passenger = max_passenger
            uber_user.special_vehicle_info = special_vehicle_info
            uber_user.save()
            return redirect('profile')
    else:
        form = DriverUpdateForm()
    return render(request,'user/drUpdate.html',{'form':form})