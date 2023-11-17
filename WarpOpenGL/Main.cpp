#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <Common/CommonTypes.h>
#include <Common/Profiler.h>
#include <Imgui/ImGuiWrapper.h>

#include <Shader/Shader.h>
#include <Cameras/FPCamera.h>
#include <Mesh/Model.h>

#include <windows.h>
#include <RenderDoc/renderdoc_app.h>

#include <Renderer/GLRenderer/GLRenderer.h>
#include <Buffers/GLBuffers/UniformBufferObject.h>
#include <Mesh/ModelLoader.h>
#include <SceneManagement/RenderPass/RenderPassDefs/RenderPassCollection.h>
#include <Shader/ShaderManager.h>
#include <SceneManagement/Scene/Scene.h>
#include <texture/GLTexture/GLTexture.h>

//Demos
#include <DemoCode/LightsOnPlane/LightsOnPlane.h>
#include <DemoCode/Asteroids/Asteroids.h>
#include <DemoCode/Backpacks/Backpacks.h>

/*TO DO spitballing goes here*/

//TODO cleanup revisit
// 1. Command recording is a priority.
// 2. Render context that can then be optimized to reduce context switches
// 3. Scene updating should have a function registry for elements in the scene that Tick
// 4. PBR, IBL, Cube Maps, post process etc

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
#ifdef _DEBUG
void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);
#endif
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

FPCamera Camera(glm::vec3(0.0f, 25.0f, 100.0f));
Ref<Scene> g_scene(nullptr);
bool bFirstMouse = true;
float LastX = SCR_WIDTH / 2.0f;
float LastY = SCR_HEIGHT / 2.0f;
float DeltaTime = 0.0f;
float LastFrame = 0.0f;

#ifdef _DEBUG
void LoadRenderDoc()
{
	RENDERDOC_API_1_1_2* rdoc_api = NULL;
	HINSTANCE hLib = LoadLibraryA("Lib\\renderdoc.dll");
	if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
	{
		pRENDERDOC_GetAPI RENDERDOC_GetAPI =
			(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
		int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);
		assert(ret == 1);
	}
}
#endif

GLFWwindow* InitGLFW(const string title, const uint width, const uint height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	LoadRenderDoc();
#endif

	GLFWwindow* window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (window == nullptr)
	{
		throw new std::runtime_error("Failed to initialize GLFW window");
		glfwTerminate();
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetScrollCallback(window, scroll_callback);

	return window;
}

void InitGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw new std::runtime_error("Failed to initialize GLAD");
	}
}

int main()
{
	Instrumentor::Get().BeginSession("Renderer");

	//init libraries
	GLFWwindow* Window = InitGLFW("Testing OpenGL", 1920, 1080);
	InitGLAD();
	ImGuiWrapper Imgui(Window);
	RenderPassCollection PassCollection;
#ifdef _DEBUG
	//g_renderer.Enable(GL_DEBUG_OUTPUT);
	g_renderer.Enable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(&MessageCallback, 0);
#endif
	g_renderer.EnableBufferBits(GL_DEPTH_BUFFER_BIT);
	g_renderer.EnableBufferBits(GL_COLOR_BUFFER_BIT);

	g_renderer.Enable(GL_DEPTH_TEST);
	g_renderer.Enable(GL_MULTISAMPLE);
	g_renderer.Enable(GL_CULL_FACE);

	Ref<Demo> CurrentDemo = std::make_shared<BackPacksDemo>();
	//Ref<Demo> CurrentDemo = std::make_shared<Asteroids>();
	//Ref<Demo> CurrentDemo = std::make_shared<LightOnPlaneDemo>();

	g_scene = CurrentDemo->MakeScene(Window);

	UniformBufferObject ViewProjUBO(2*sizeof(glm::mat4), 0);
	PassCollection.AddGBufferPass(*g_scene);
	PassCollection.AddGBufferLightingPass(*g_scene);

	//auto textures = loader.getTextures();
	//passCollection.AddDebugQuadDraw(textures);
	//auto& shaders = passCollection.getShaderManager().getShaders();
	//passCollection.AddZOnlyPass(1024, 1024, models);
	//passCollection.AddOpaquePass(*g_scene);
	// 
	// render loop
	// -----------
	while (!glfwWindowShouldClose(Window))
	{
		float currentFrame = glfwGetTime();
		DeltaTime = currentFrame - LastFrame;
		LastFrame = currentFrame;

		//Vector<Ref<Shader>>& shaders = passCollection.getShaderManager().getShaders();
		//imgui.newFrame();
		//ImGui::Begin("Performance Monitor");
		//ImGui::Text("Frame Time: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//for (uint i = 0; i < shaders.size(); ++i)
		//{
		//	auto& shader = shaders[i];
		//	ImGui::PushID(i);
		//	ImGui::Text(shader->getName().c_str());
		//	ImGui::SameLine();
		//	if (ImGui::Button("Recompile"))
		//	{
		//		shader->recompile();
		//	}
		//	ImGui::PopID();
		//}
		//ImGui::End();

		// input
		// -----
		processInput(Window);

		//update
		if (g_scene->HasWindowChanged())
		{
			g_renderPassManager.ExecuteSetups();
		}

		glm::mat4 Projection = glm::perspective(glm::radians(g_scene->GetActiveCamera().GetZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 5000.0f);
		glm::mat4 View = g_scene->GetActiveCamera().GetViewMatrix();

		ViewProjUBO.BindSubdata(Projection);
		ViewProjUBO.BindSubdata(View);

		g_scene->Update(DeltaTime);
		CurrentDemo->Update(DeltaTime);
		// render
		// ------
		g_renderPassManager.ExecutePasses();

		//imgui.render();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		{
			PROFILE_SCOPE("Swap Buffers");
			glfwSwapBuffers(Window);
			glfwPollEvents();
		}
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();

	Instrumentor::Get().EndSession();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
	{
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			g_scene->GetActiveCamera().ProcessKeyboard(SHIFT, DeltaTime);
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			g_scene->GetActiveCamera().ProcessKeyboard(FORWARD, DeltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			g_scene->GetActiveCamera().ProcessKeyboard(BACKWARD, DeltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			g_scene->GetActiveCamera().ProcessKeyboard(LEFT, DeltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			g_scene->GetActiveCamera().ProcessKeyboard(RIGHT, DeltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
	{
		//GLFW_CURSOR_NORMAL 
		if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
		{
			glfwSetCursorPosCallback(window, nullptr);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			bFirstMouse = true;
			glfwSetCursorPosCallback(window, mouse_callback);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
	g_scene->SetWindowChanged(true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (bFirstMouse)
	{
		LastX = xpos;
		LastY = ypos;
		bFirstMouse = false;
	}

	float xoffset = xpos - LastX;
	float yoffset = LastY - ypos; // reversed since y-coordinates go from bottom to top

	LastX = xpos;
	LastY = ypos;

	g_scene->GetActiveCamera().ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	g_scene->GetActiveCamera().ProcessMouseScroll(yoffset);
}

#ifdef _DEBUG
void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
#endif
