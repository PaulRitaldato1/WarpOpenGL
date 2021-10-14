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
#include <Mesh/GeoGen.h>
#include <Mesh/ModelLoader.h>
#include <SceneManagement/RenderPass/RenderPassDefs/RenderPassCollection.h>
#include <Shader/ShaderManager.h>
#include <SceneManagement/Scene/Scene.h>

/*TO DO spitballing goes here*/

//TODO REGISTER SYSTEM FOR IMGUI. SORT OF LIKE VISITOR PATTERN BUT FOR ANY REGISTERED VAR
//TODO LIST OF RENDER ITEMS THAT CAN BE SORTED BY DISTANCE TO CAMERA
//TODO SHOULD ALL BUFFER ALLOC GO THROUGH RENDERER? 
//	1. COULD MAKE DRAW CALLS EASIER?
//TODO create psuedo command lists. Render passes record commands into a vector
// without making any GL calls. Then render thread executes these commands with the current context
// this should allow asyn render passes
//TODO materials and lighting. Already being worked on but needs a lot of work

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

FPCamera camera(glm::vec3(0.0f, 0.0f, 10.0f));
bool firstMouse = true;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

#ifdef _DEBUG
void loadRenderDoc()
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

GLFWwindow* initGLFW(const string title, const uint width, const uint height)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef _DEBUG
	loadRenderDoc();
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

void initGLAD()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		throw new std::runtime_error("Failed to initialize GLAD");
	}
}

Vector<glm::mat4> generateAsteroidPositions(uint amount)
{
	Vector<glm::mat4> modelMatrices;

	srand(glfwGetTime()); // initialize random seed	
	float radius = 150.0;
	float offset = 25.0f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with 'radius' in range [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: Scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. now add to list of matrices
		modelMatrices.push_back(model);
	}

	return modelMatrices;
}

int main()
{
	Instrumentor::Get().BeginSession("Renderer");

	//init libraries
	GLFWwindow* window = initGLFW("Testing OpenGL", 1920, 1080);
	initGLAD();
	ImGuiWrapper imgui(window);
	ModelLoader loader("Resources/");
	RenderPassCollection passCollection;
	//GLRenderer g_renderer;
#ifdef _DEBUG
	g_renderer.Enable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);
#endif
	g_renderer.EnableBufferBits(GL_DEPTH_BUFFER_BIT);
	g_renderer.EnableBufferBits(GL_COLOR_BUFFER_BIT);

	g_renderer.Enable(GL_DEPTH_TEST);
	g_renderer.Enable(GL_MULTISAMPLE);
	g_renderer.Enable(GL_CULL_FACE);

	//uint amount = 100000;
	//Vector<glm::mat4> modelMatrices = generateAsteroidPositions(amount);
	
	std::vector<glm::vec3> objectPositions;
	objectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	objectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	objectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	Vector<glm::mat4> modelMatrices;
	glm::mat4 base = glm::mat4(1.0f);
	base = glm::scale(base, glm::vec3(1.0f, 1.0f, 1.0f));

	for (auto& pos : objectPositions)
		modelMatrices.push_back(glm::translate(base, pos));

	//load model
	Vector<ModelDesc> modelDescs;
	modelDescs.push_back({"Resources/Backpack/backpack.obj", false, true, modelMatrices});
	//modelDescs.emplace_back("Resources/planet/planet.obj", false, true, model);
	//modelDescs.emplace_back("Resources/rock/rock.obj", false, true, modelMatrices);
	Vector<Ref<Model>> models = loader.loadModelsAsync(modelDescs);
	
	Vector<Pointlight> pointLights;
	pointLights.emplace_back(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 100.0f, glm::vec3(1.0f, 1.0f, 1.0f));
	//pointLights.emplace_back(glm::vec3(1.0f, 0.0f, 5.0f), 10.0f, 10.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	
	Vector<Spotlight> spotlights;

	Vector<DirectionalLight> directionalLights;
	directionalLights.emplace_back(glm::vec3(.33f, .50f, .33), glm::vec3(-0.2f, -1.0f, -0.3f), false);

	Vector<FPCamera> cameras;
	cameras.push_back(camera);

	Scene scene("BasicBackpack", spotlights, pointLights, directionalLights, models, cameras);

	UniformBufferObject viewProjUBO(2*sizeof(glm::mat4), 0);

	passCollection.AddGBufferPass(scene);
	passCollection.AddGBufferLightingPass(scene);

	//auto textures = loader.getTextures();
	//passCollection.AddDebugQuadDraw(textures);
	auto& shaders = passCollection.getShaderManager().getShaders();
	//passCollection.AddZOnlyPass(1024, 1024, models);
	//passCollection.AddOpaquePass(models);
	// 
	// render loop
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		imgui.newFrame();
		ImGui::Begin("Performance Monitor");
		ImGui::Text("Frame Time: %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		for (uint i = 0; i < shaders.size(); ++i)
		{
			auto& shader = shaders[i];
			ImGui::PushID(i);
			ImGui::Text(shader->getName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Recompile"))
			{
				shader->recompile();
			}
			ImGui::PopID();
		}
		ImGui::End();


		// input
		// -----
		processInput(window);

		//update
		glm::mat4 projection = glm::perspective(glm::radians(camera.getZoom()), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
		glm::mat4 view = camera.getViewMatrix();

		viewProjUBO.BindSubdata(projection);
		viewProjUBO.BindSubdata(view);

		// render
		// ------

		g_renderPassManager.ExecutePasses();

		imgui.render();
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		{
			PROFILE_SCOPE("Swap Buffers");
			glfwSwapBuffers(window);
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
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.processKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.processKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.processKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.processKeyboard(RIGHT, deltaTime);
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
			firstMouse = true;
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
}

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

	camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

#ifdef _DEBUG
void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n\n",
		(type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}
#endif
