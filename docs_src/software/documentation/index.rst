Sphinx documentation
------------------------------------

This documentation is generated using `Sphinx <https://www.sphinx-doc.org/en/master/>`__, and automatically deployed to github pages

How-to build
___________________________________

.. important::

   You need to be on a linux system with docker installed

.. code-block::

   cd docs_src && ./build.sh

How-to edit
___________________________________

You can also build the documentation in dev mode with 

.. code-block::

   cd docs_src && ./build.sh --dev

In this mode, the documentation is served on localhost:8001, and automatically updated when you make a change to the sources