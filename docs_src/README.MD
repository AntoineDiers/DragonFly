# Documentation

This documentation is generated using [Sphinx](https://www.sphinx-doc.org/en/master/), and automatically deployed to github pages

### How-to build

> You need to be on a linux system with docker installed

```
cd docs_src && ./build.sh
```

###  How-to edit

You can also build the documentation in dev mode with 

```
cd docs_src && ./build.sh --dev
```

In this mode, the documentation is served on localhost:8001, and automatically updated when you make a change to the sources