from distutils.core import setup, Extension
setup(name="andiDB", version="1.0",
      ext_modules=[
          Extension("andiDB", ["andiDBValue.c", "andiDBClient.c"])]
      )
