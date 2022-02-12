from django.shortcuts import render,redirect
from user.models import UberUser
from .models import Ride, Party
from django.contrib.auth.decorators import login_required
# from django.contrib.auth.models import User
from .forms import rideRequestForm, sharerSearchForm, sharerRequestForm
from .decorators import require_customer, require_driver
from django.shortcuts import get_object_or_404
from django.db.models import F
from django.db.models import Q
from django.contrib import messages
from django.core.mail import send_mail
from django.template.loader import render_to_string

# Home
def home(request):
    return render(request, 'home.html')

##################### For Owner #############################
# Get the Ride information as dictionary
def getRideInformation(ride):
    ctx = dict()
    ctx['id'] = ride.id
    ctx['owner'] = ride.owner.username
    ctx['driver'] = '' if ride.driver is None else ride.driver.username
    ctx['sharers'] = list()
    for sharer in ride.sharer.all():
        ctx['sharers'].append(sharer.username)
    ctx['from'] = ride.from_location
    ctx['dest'] = ride.dest_location
    ctx['arr_date_time'] = ride.arr_date_time
    ctx['num_passenger'] = ride.max_passenger
    ctx['license_plate_number'] = ride.license_plate_number
    ctx['vehicle_type'] = '' if ride.vehicle_type is None else ride.vehicle_type
    ctx['special_vehicle_info'] = '' if ride.special_vehicle_info is None else ride.special_vehicle_info
    ctx['share_flag'] = ride.share_flag
    ctx['is_confirmed'] = ride.is_confirm
    ctx['is_completed'] = ride.is_complete
    return ctx

# Check ride status
@login_required
def rideStatus(request, ride_id):
    ride = get_object_or_404(Ride, pk=ride_id)
    # Only the ride owner, sharer or driver could viwe the ride status.
    is_driver = (ride.driver is not None) and (ride.driver.id == request.user.id)
    is_owner = ride.owner.id == request.user.id
    is_sharer = False
    for sharer in ride.sharer.all():
        if sharer.id == request.user.id:
            is_sharer = True
            break
    if is_driver or is_owner or is_sharer:
        ctx = getRideInformation(ride)
        ctx['is_sharer_req'] = False
        ctx['button_text'] = 'Do not press'
        ctx['sharer_req_form'] = None
        is_ride_sharer, _ = check_user_is_ride_sharer(request.user, ride)
        ctx['is_sharer_joined'] = False
        if is_ride_sharer:
            ctx['is_sharer_joined'] = True
        return render(request, 'ride/rideStatus.html', ctx)
    messages.error(request, 'You are not permitted to view this ride information.')
    return redirect('home')

# Send the email for notification
def send_email_helper(user, msg, title):
    msg_html = render_to_string('notification_email_template.html', {
        'username': user.username,
        'msg': msg
    })
    send_mail(
        title,
        msg_html,
        'uber.ride.hw1@gmail.com',
        [user.email]
    )

# Start a new Ride with required information
@login_required
@require_customer
def createRideRequest(request):
    if request.method == 'POST':
        form = rideRequestForm(request.POST)
        if form.is_valid():
            ride = form.save(commit=False)
            ride.owner = request.user
            ride.is_confirm = False
            ride.is_complete = False
            ride.save()
            # Create Party for each new ride
            party = Party(user=request.user, num_passenger=ride.max_passenger, ride=ride, joined_as='O')
            party.save()
            messages.success(request, 'Ride request created, waiting for a driver to comfirm the ride')
            return redirect('ride-status', ride_id=ride.id)
    else:
        form = rideRequestForm()
    return render(request, 'ride/createRideRequest.html', {'form': form})

# Owner could edit the ride status before driver confirmed a ride
@login_required
@require_customer
def ownerEditRideStatus(request, ride_id):
    ride = get_object_or_404(Ride, pk=int(ride_id))
    original_dest = ride.dest_location
    form = rideRequestForm(request.POST or None, instance=ride)
    if request.method == 'POST':
        # check if user is ride owner
        if form.is_valid():
            owner = request.user
            if ride.owner.id != request.user.id:
                return redirect('home')
            # owner changed destination -> all sharers should be removed and
            #  be notified of cancellation
            new_arr_datetime = form.cleaned_data['arr_date_time']
            is_sharer_removed = False
            for pt in ride.ride_party.all():
                if pt.joined_as == 'S':
                    if (form.cleaned_data['dest_location'] != original_dest) or (
                            new_arr_datetime < pt.start_time or new_arr_datetime > pt.end_time):
                        if not is_sharer_removed:
                            is_sharer_removed = True
                        sharer_passenger = pt.num_passenger
                        ride.max_passenger = ride.max_passenger - sharer_passenger
                        ride.sharer.remove(pt.user)
                        ride.save()
                        send_email_helper(pt.user,
                                          'We are sorry to nofify you that due to some changes your ride share request is canceled',
                                          'Uber ride hw1: Ride share canceled')

                        pt.delete()

            form.save()
            if is_sharer_removed:
                messages.info(request, 'Sharers were removed from the ride due to changes you made')
            return redirect('ride-status', ride_id=ride.id)
    return render(request, 'ride/forms.html',
                  {'form': form, 'title': 'Edit ride status', 'button_text': 'Update ride status'})

##################### For Sharer #############################

# Check if the user is ride sharer in Party, if it is, return the sharer info in the Party
def check_user_is_ride_sharer(user, ride):
    for pt in ride.ride_party.all():
        if pt.joined_as == 'S' and pt.user.id == user.id:
            return True, pt
    return False, None

# Sharer provide information to get response with match open rides
@login_required
@require_customer
def sharerSelect(request):
    if request.method == 'POST':
        form = sharerSearchForm(request.POST)
        if form.is_valid():
            dest_location = form.cleaned_data['dest_location']
            start_time = form.cleaned_data['start_time']
            end_time = form.cleaned_data['end_time']
            max_passenger = form.cleaned_data['max_passenger']
            rides_open = Ride.objects.filter(
                ~Q(owner=request.user),
                ~Q(driver=request.user),
                is_confirm=False,
                arr_date_time__lte=end_time,
                arr_date_time__gte=start_time,
                share_flag=True,
                dest_location=dest_location,
            ).all()
            return render(request,'ride/openrides.html',{'rides':rides_open})
    else:
        form = sharerSearchForm()
    return render(request, 'ride/sharerRideRequest.html', {'form': form})

# Sharer accept the rides from previous openrides list
@login_required
@require_customer
def sharerAcceptRide(request, ride_id):
    ride = Ride.objects.get(id=ride_id)
    ride.sharer.add(request.user)
    ride.save()
    return redirect('ride-status', ride_id=ride_id)

# Sharer could quit Ride if required, will delete the corresponding information accordingly
@login_required
@require_customer
def sharerQuitRide(request, ride_id):
    ride = get_object_or_404(Ride, pk=ride_id)
    is_sharer, pt = check_user_is_ride_sharer(request.user, ride)
    if is_sharer:
        sharer_passenger = pt.num_passenger
        ride.max_passenger = ride.max_passenger - sharer_passenger
        ride.sharer.remove(pt.user)
        pt.delete()
        ride.save()
        messages.success(request, 'Quited ride as sharer')
        return redirect('home')
    else:
        messages.error('Can not quit this ride as sharer')
    return redirect('home')
    
# Save the corresponding information for the ride which selected by sharer, redirect to ride status.
@login_required
@require_customer
def rideStatusForSharer(request, ride_id):
    ride = get_object_or_404(Ride, pk=ride_id)
    if request.method == 'POST':
        form = sharerRequestForm(request.POST)
        if form.is_valid():
            num_passenger = form.cleaned_data['num_passenger']
            start_time = form.cleaned_data['start_time']
            end_time = form.cleaned_data['end_time']
            pt = Party(user=request.user, num_passenger=num_passenger, ride=ride, joined_as='S', start_time=start_time, end_time=end_time)
            pt.save()
            ride.sharer.add(request.user)
            ride.max_passenger += num_passenger
            ride.save()
            messages.success(request, 'Successfully joined ride as sharer')
            return redirect('home')
        messages.error(request, 'Bad request for trying to join ride as sharer')
        return redirect('home')
    form = sharerRequestForm()
    ctx = getRideInformation(ride)
    ctx['is_sharer_req'] = True
    ctx['sharer_req_form'] = form
    ctx['button_text'] = 'Join as sharer'
    ctx['is_sharer_joined'] = False
    return render(request, 'ride/rideStatus.html', ctx)

##################### For Driver #############################

# Driver fill the form with needed information, response to a list of open rides matches the info.
@login_required
@require_driver
def driverSelect(request):
    user_driver = request.user
    rides_open = Ride.objects.filter(~Q(owner=user_driver),~Q(sharer=user_driver),max_passenger__lte = user_driver.max_passenger,is_confirm = False)
    open_rides = list()
    for ride in rides_open:
        if ride.vehicle_type is not None and ride.vehicle_type != '' and ride.vehicle_type != user_driver.vehicle_type:
            continue

        if ride.special_vehicle_info is not None and ride.special_vehicle_info != '' and ride.special_vehicle_info != user_driver.special_vehicle_info:
            continue
        open_rides.append(ride)
    return render(request,'ride/openrides.html',{'rides':open_rides})

# Driver accept the selected ride, ride confirmed
@login_required
@require_driver
def acceptRideRequest(request,ride_id):
    ride = Ride.objects.get(id=ride_id)
    ride.driver = request.user
    ride.vehicle_type = request.user.vehicle_type
    ride.license_plate_number = request.user.license_plate_number
    ride.is_confirm = True
    msg = f'Your ride from {ride.from_location} to {ride.dest_location} have been confirmed by a driver'
    title = 'Uber ride hw1: ride confirmed'
    send_email_helper(ride.owner, msg, title)
    for s in ride.sharer.all():
        send_email_helper(s, msg, title)
    ride.save()
    return redirect('ride-status', ride_id=ride_id)

# Driver select to complete the ride
@login_required
@require_driver
def completeDrive(request, ride_id):
    ride = Ride.objects.get(id=ride_id)
    if ride.driver == request.user and request.user.user_type == 'DR':
        ride.is_complete = True
        ride.save()
        return redirect('ride-status', ride_id=ride_id)
    messages.error(request, 'Sorry, you cannot complete the ride at this time')
    return redirect('home')


##################### For Rides Information #############################

# Current Rides include open rides, non-complete rides.
def currentRides(request):
    return render(request, 'ride/currentRides.html')

# Current Rides for Customer
def currentRidesCustomer(request):
    open_rides = Ride.objects.filter(Q(owner=request.user) | Q(sharer=request.user),is_confirm=False)
    non_complete_rides = Ride.objects.filter(Q(owner=request.user) | Q(sharer=request.user),is_confirm=True, is_complete=False)
    context = {
        'o_rides': open_rides,
        'nc_rides': non_complete_rides
    }
    return render(request, 'ride/currentRides-customer.html', context)

# Current Rides for Driver
def currentRidesDriver(request):
    non_complete_rides = Ride.objects.filter(driver=request.user,is_confirm=True, is_complete=False)
    return render(request, 'ride/currentRides-driver.html', {'rides':non_complete_rides})

# CompleteRides lists following the role
def completeRides(request):
    as_driver = Ride.objects.filter(driver=request.user, is_complete=True)
    as_owner = Ride.objects.filter(owner=request.user, is_complete=True)
    as_sharer = Ride.objects.filter(sharer=request.user, is_complete=True)
    context = {
        'as_driver':as_driver,
        'as_owner':as_owner,
        'as_sharer':as_sharer
    }
    return render(request, 'ride/completeRides.html',context)
