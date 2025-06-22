# mkdocs

The `mkdocs` project part contains the

  - `mkdocs.yml.template`
  
    This will be automatically used to generate the final mkdocs.yml with `make mkdocs`
    (or one of its variants) in the parent directory
    
  - `docs`

    It does not contain the actual docs and is only meant to hold the extra `css` or `js`
    files because these do not belong with the content in the `src` directories of the
    project.
  
  - `pdm` related files
  
    If a package/project management tool like `pdm` is used for the python virtual
    environment and package management/installation to have `mkdocs` (and
    `mkdocs-material`) work, these files will be here.
