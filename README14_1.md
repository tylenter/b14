CÁC BƯỚC THỰC HIỆN:
1. Cấu hình USART:
   - Mục đích: Cấu hình chân PA9 (TX) và PA10 (RX) để giao tiếp UART.
   - Sử dụng thư viện chuẩn của STM32 để thiết lập GPIO và USART.
   - Tốc độ baud: 115200.

  <img width="999" height="555" alt="image" src="https://github.com/user-attachments/assets/1480e58a-bae8-42b6-86f2-b829834ec8db" />

2. Hàm gửi USART:
   - Tạo hàm UART1_SendString để gửi chuỗi qua UART.
   - Sử dụng Mutex (xUARTMutex) để bảo vệ tài nguyên UART.
   - xSemaphoreTake: Đảm bảo chỉ một task được sử dụng UART tại một thời điểm.
   - xSemaphoreGive: Giải phóng UART cho task khác sử dụng.
  
<img width="836" height="336" alt="image" src="https://github.com/user-attachments/assets/ac066c37-3ac5-48de-8360-033bab785d55" />


3. Định nghĩa Task:
   - Tạo các task TaskA, TaskB, TaskC hoạt động độc lập.
   - Mỗi task sẽ chờ (block) tại hàm xEventGroupWaitBits.
   - TaskA chờ BIT_TASK_A, TaskB chờ BIT_TASK_B, TaskC chờ BIT_TASK_C.
   - Tham số pdTRUE: Các bit này sẽ tự động bị xóa (clear) sau khi task được đánh thức.
   - Khi nhận được bit tương ứng, task sẽ chạy và gửi thông báo qua UART.
  
  <img width="972" height="396" alt="image" src="https://github.com/user-attachments/assets/1373b437-010f-4029-996c-73fa31e9b793" />

<img width="987" height="202" alt="image" src="https://github.com/user-attachments/assets/7859525e-85a8-47fb-807c-b5c09494dfc0" />

4. Định nghĩa Task Control:
   - TaskControl dùng để điều phối việc thực thi của các task A, B, C.
   - Sử dụng xEventGroupSetBits để gửi (set) các bit sự kiện, đánh thức các task tương ứng.
   - Logic:
       + Kích hoạt TaskA, chờ 1 giây.
       + Kích hoạt TaskB, chờ 1 giây.
       + Kích hoạt TaskC, chờ 1 giây.
       + Kích hoạt cả TaskA, TaskB, TaskC cùng lúc, chờ 3 giây.
       + Lặp lại.
    
  <img width="944" height="440" alt="image" src="https://github.com/user-attachments/assets/9ac4e69d-5402-4978-ad9b-f2769205c8b3" />

5. Main:
   - Khởi tạo hệ thống (SystemInit) và UART1_Init.
   - Tạo xUARTMutex bằng xSemaphoreCreateMutex.
   - Tạo xEventGroup bằng xEventGroupCreate.
   - Tạo 4 task: TaskA, TaskB, TaskC (ưu tiên 2) và TaskControl (ưu tiên 3).
   - Bắt đầu bộ lập lịch FreeRTOS với vTaskStartScheduler.
  
<img width="823" height="368" alt="image" src="https://github.com/user-attachments/assets/f3b8d051-aad5-45f1-a93a-b91ab2a0f904" />

6. Kết quả:
   - Chương trình sẽ tuần tự in ra output:
   - "Kich hoat Task A" -> "Task A dang chay"
   - "Kich hoat Task B" -> "Task B dang chay"
   - "Kich hoat Task C" -> "Task C dang chay"
   - "Kich hoat dong thoi A + B + C" -> "Task A dang chay", "Task B dang chay", "Task C dang chay" .

