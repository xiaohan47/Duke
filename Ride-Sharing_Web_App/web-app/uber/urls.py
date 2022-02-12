"""uber URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/4.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path
from user import views as user_views
from django.contrib.auth import views as auth_views
from ride import views as ride_views
from django.views.generic import TemplateView

urlpatterns = [
    path('',ride_views.home,name='home'),
    path('home/', ride_views.home, name='home'),
    path('admin/', admin.site.urls),
    #user
    path('register/',user_views.register,name='register'),
    path('profile/',user_views.profile,name='profile'),
    path('ctprofile/',user_views.customerUpdate,name='custom-update'),
    path('drprofile/',user_views.driverUpdate,name='driver-update'),
    path('login/',auth_views.LoginView.as_view(template_name='user/login.html'),name='login'),
    path('logout/',auth_views.LogoutView.as_view(template_name='user/logout.html'),name='logout'),
    path('role-switch/', user_views.roleSwitch, name='role-switch'),
    path('driver-register/',user_views.driverRegister,name='driver-register'),
    #rides
    path('ride-request/',ride_views.createRideRequest,name = 'ride-request'),
    path('ride-status/', TemplateView.as_view(template_name='rideStatus.html'), name='ride-status'),
    path('driver-open-rides/', ride_views.driverSelect, name='driver-open-rides'),
    path('accept-ride-req/<int:ride_id>', ride_views.acceptRideRequest, name='acc-ride-req'),
    path('complete-drive/<int:ride_id>', ride_views.completeDrive, name='complete-drive'),
    path('owner-edit-ride/<int:ride_id>', ride_views.ownerEditRideStatus, name='owner-edit-ride'),
    path('ride-status/<int:ride_id>', ride_views.rideStatus, name='ride-status'),
    path('current-rides',ride_views.currentRides,name = 'current-rides'),
    path('current-rides-customer',ride_views.currentRidesCustomer,name = 'current-rides-customer'),
    path('current-rides-driver',ride_views.currentRidesDriver,name = 'current-rides-driver'),
    path('complete-rides',ride_views.completeRides,name = 'complete-rides'),
    # sharer
    path('sharer-ride-request/',ride_views.sharerSelect,name='sharer-ride-request'),
    path('sharer-accept-ride/<int:ride_id>',ride_views.sharerAcceptRide,name='sharer-accept-ride'),
    path('sharer-req-ride-status/<int:ride_id>', ride_views.rideStatusForSharer, name='sharer-req-ride-status'),
    path('sharer-quit-ride/<int:ride_id>', ride_views.sharerQuitRide, name='sharer-quit-ride'),
]
