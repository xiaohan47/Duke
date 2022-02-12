from django.core.exceptions import PermissionDenied
from user.models import UberUser

def require_driver(view_func):
    def wrapper(request, *args, **kwargs):
        qs = UberUser.objects.filter(id=request.user.id)
        if not qs:
            raise PermissionDenied()
        else:
            uber_user = qs.first()
            if uber_user.user_type != 'DR':
                raise PermissionDenied()
            else:
                return view_func(request, *args, **kwargs)
    wrapper.__doc__ = view_func.__doc__
    wrapper.__name__ = view_func.__name__
    return wrapper

def require_customer(view_func):
    def wrapper(request, *args, **kwargs):
        qs = UberUser.objects.filter(id=request.user.id)
        if not qs:
            raise PermissionDenied()
        else:
            uber_user = qs.first()
            if uber_user.user_type != 'CT':
                raise PermissionDenied()
            else:
                return view_func(request, *args, **kwargs)
    wrapper.__doc__ = view_func.__doc__
    wrapper.__name__ = view_func.__name__
    return wrapper
