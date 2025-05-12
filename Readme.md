# RTSP Analyser

Analyse video stream to detect human.

Make lightweight to be execute on RPI 4.

## Status

Under development, not ready.

## Workflow

![Workflow](./doc/uml/out/workflow.png)

## Dependencies

- OpenCV 4
- Niels Lohmann JSON
- Boost
- Boost Asio
- fmt

## TO DO

- Full usage of dependencies from vcpkg include on ARM architecture
- Test & Impl Mouvement detectection                Testing
- Test & Impl Human detection                       Dev
- Impl SMTP client to send e-mail
- Use JSON for config file                          OK
- ...
