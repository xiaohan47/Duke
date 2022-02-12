from lib2to3.pytree import Base
from django.contrib.auth.base_user import BaseUserManager

class UberUserManager(BaseUserManager):
    use_in_migrations = True

    def create_user(self, username, email, password):

        if not email:
            raise TypeError('Email cannot be blank')
        
        user = self.model(username=username, email=self.normalize_email(email))
        # email = self.normalize_email(email)
        # user = self.model(email=email, **extra_fields)
        # user.set_password(password)
        # user.save(using=self._db)
        return user
    
    def create_superuser():
        pass