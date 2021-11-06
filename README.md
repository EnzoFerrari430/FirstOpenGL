# FirstOpenGL
一个很偶然的机会让我遇到了OpenGL 了解到了图形学，GPU编程，并产生了极大的兴趣，遂开始学习。 希望以后能创作出有意思的东西

## 仓库目录
1. OpenGL  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这个工程里是我第一次接触OpenGL建立的学习工程，教程来自B站 [最好的OpenGL教程之一](https://www.bilibili.com/video/BV1MJ411u7Bc?spm_id_from=333.999.0.0)  
课程来自国外，视频里是用英文进行讲解，但是吐字准确，一般都听得懂。 课程也比较简单清晰易懂，跟着学下来没遇到特别的难点。  
帮助我跨过了前期配置OpenGL的巨量问题，画出了人生的第一个三角形。目前学至p22  

2. LearnOpenGL  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这个工程是我后续接触的到的另一个学习教程，教程来自 [LearnOpenGL CN](https://learnopengl-cn.github.io/)。目前是我主要的学习课程，了解到了很多更细更广的知识。  
    1. FirstOpenGL项目  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这个项目对应教程中的入门篇  
参考的网上资料有 [矩阵旋转](https://www.cnblogs.com/zhoug2020/p/7842808.html) [视点矩阵推导lookAt](https://zhuanlan.zhihu.com/p/356878461)  
在入门篇的摄像机小节里，对教程的鼠标移动做了小修改  

    	1. 原文全局变量

			```
			bool firstMouse = true;
			float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
			float pitch =  0.0f;
			float lastX =  800.0f / 2.0;
			float lastY =  600.0 / 2.0;
			float fov   =  45.0f;
			```

		2. 原文鼠标相应函数

			```
	   		void mouse_callback(GLFWwindow* window, double xpos, double ypos)
			{
				if (firstMouse)
				{
					lastX = xpos;
					lastY = ypos;
					firstMouse = false;
				}

				float xoffset = xpos - lastX;
				float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.1f; // change this value to your liking
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				yaw += xoffset;
				pitch += yoffset;

				// make sure that when pitch is out of bounds, screen doesn't get flipped
				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front;
				front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);
			}
			```

		3. 我的全局变量

			```
			float yaw = 0.0f;//偏航角  摄像机初始在正X轴 -90度之后就在正Z轴上了
			float pitch = 0.0f;//俯仰角
			float lastX = 800.0f / 2.0;
			float lastY = 600.0f / 2.0;
			float fov = 45.0f;
			bool firstMove = true;
			```

		4. 我的鼠标相应函数

			```
			void mouse_callback(GLFWwindow* window, double xpos, double ypos)
			{
				if (firstMove)
				{
					lastX = xpos;
					lastY = ypos;
					firstMove = false;
				}

				float xoffset = xpos - lastX;//鼠标左移 xpos变小 视角要左转(相机逆时针 yaw变大)
				float yoffset = lastY - ypos;//鼠标上移 ypos变小 视角要上仰(相机从下面往上看 ZOY平面是个顺时针移动 pitch变小)
				lastX = xpos;
				lastY = ypos;

				float sensitivity = 0.1f;
				xoffset *= sensitivity;
				yoffset *= sensitivity;

				yaw += xoffset;
				pitch += yoffset;

				if (pitch > 89.0f)
					pitch = 89.0f;
				if (pitch < -89.0f)
					pitch = -89.0f;

				glm::vec3 front(1.0f);
				front.x = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
				front.y = sin(glm::radians(pitch));
				front.z = -cos(glm::radians(yaw)) * cos(glm::radians(pitch));
				cameraFront = glm::normalize(front);

			}
			```

	2. Chapter2   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这个项目对应教程中的光照篇
