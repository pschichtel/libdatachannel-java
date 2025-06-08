#!/usr/bin/env sh

predefined_profile='/conan-profile.ini'

if [ -e "$predefined_profile" ]
then
  home="${CONAN_HOME?no conan home!}"
  profile_path="$home/profiles/default"
  mkdir -vp "$(dirname "$profile_path")"
  cp -av "$predefined_profile" "$profile_path"
else
  conan profile detect -f
fi
