include(FetchContent)

FetchContent_Declare(
  rapidcsv
  GIT_REPOSITORY https://github.com/d99kris/rapidcsv
  GIT_TAG v8.80
)

FetchContent_MakeAvailable(rapidcsv)