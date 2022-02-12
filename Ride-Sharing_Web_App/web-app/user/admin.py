from django.contrib import admin
from django.contrib.auth.admin import UserAdmin
from .models import UberUser
from .forms import UserRegisterForm

class UberUserAdmin(UserAdmin):
    add_form = UserRegisterForm
    model = UberUser
    list_display = ['username', 'email', 'user_type', 'vehicle_type', 'license_plate_number', 'max_passenger', 'special_vehicle_info']
    fieldsets = UserAdmin.fieldsets + (
        (None, {'fields': ('user_type', 'vehicle_type', 'license_plate_number', 'max_passenger', 'special_vehicle_info',)}),
    )

# Register your models here.
admin.site.register(UberUser, UberUserAdmin)
