import notification_service;

auto main() -> int {
  web::NotificationService service{10'000};
  service.Run();
  return 0;
}
