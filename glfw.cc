#define GLFW_INCLUDE_NONE
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// Imgui extension
#include <implot.h>

// Skia support
#define SK_GANESH
#define SK_GL
#include "include/gpu/ganesh/GrBackendSurface.h"
#include "include/gpu/ganesh/GrDirectContext.h"
#include "include/gpu/ganesh/gl/GrGLInterface.h"
#include "include/gpu/ganesh/gl/GrGLAssembleInterface.h"
#include "include/gpu/ganesh/SkSurfaceGanesh.h"
#include "include/gpu/ganesh/gl/GrGLBackendSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorSpace.h"
#include "include/core/SkSurface.h"

#include "include/gpu/ganesh/gl/GrGLDirectContext.h"

#include "include/core/SkFont.h"
#include "include/core/SkString.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkStream.h"
#include "include/core/SkRRect.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkPathEffect.h"

#include "include/effects/Sk1DPathEffect.h"


// Setup Dear ImGui context
void setupImGui(GLFWwindow* window) {
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130"); // Specify your OpenGL version here
}

#include <iostream>

void renderImGui() {
    // Example ImGui window
    ImGui::Begin("Hello, ImGui!");
    ImGui::Text("This is a simple GUI example.");
    ImGui::End();
}


// [Skia setup](https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add)
GrDirectContext* sContext = nullptr;
SkSurface* sSurface = nullptr;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void init_skia(int w, int h) {
	auto interface = GrGLMakeNativeInterface();
	if (interface == nullptr) {
		//backup plan. see https://gist.github.com/ad8e/dd150b775ae6aa4d5cf1a092e4713add?permalink_comment_id=4680136#gistcomment-4680136
		interface = GrGLMakeAssembledInterface(
			nullptr, (GrGLGetProc) * [](void*, const char* p) -> void* { return (void*)glfwGetProcAddress(p); });
	}
	sContext = GrDirectContexts::MakeGL(interface).release();

	GrGLFramebufferInfo framebufferInfo;
	framebufferInfo.fFBOID = 0; // assume default framebuffer
	// We are always using OpenGL and we use RGBA8 internal format for both RGBA and BGRA configs in OpenGL.
	//(replace line below with this one to enable correct color spaces) framebufferInfo.fFormat = GL_SRGB8_ALPHA8;
	framebufferInfo.fFormat = GL_RGBA8;

	SkColorType colorType = kRGBA_8888_SkColorType;
	GrBackendRenderTarget backendRenderTarget = GrBackendRenderTargets::MakeGL(w, h,
		0, // sample count
		0, // stencil bits
		framebufferInfo);

	//(replace line below with this one to enable correct color spaces) sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, SkColorSpace::MakeSRGB(), nullptr).release();
	sSurface = SkSurfaces::WrapBackendRenderTarget(sContext, backendRenderTarget, kBottomLeft_GrSurfaceOrigin, colorType, nullptr, nullptr).release();
	if (sSurface == nullptr) abort();
}

// Function to draw a path using a circular brush
void drawPathWithCircularBrush(SkCanvas* canvas, const SkPath& path, float circleRadius, float spacing) {
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setColor(SK_ColorBLACK);

    // Create a circular path to use as the brush
    SkPath circleBrush;
    circleBrush.addCircle(0, 0, circleRadius);

    // Create a PathEffect to place the circle along the path at regular intervals
    // SkPath1DPathEffect::Make will create the brush effect
    sk_sp<SkPathEffect> pathEffect = SkPath1DPathEffect::Make(circleBrush, spacing, 0, SkPath1DPathEffect::kRotate_Style);

    // Apply the path effect to the paint
    paint.setPathEffect(pathEffect);

    // Draw the path with the path effect applied
    paint.setARGB(128, 0, 0, 255);  // 50% transparent blue
    canvas->drawPath(path, paint);
}

void drawExample(SkCanvas* canvas) {
    canvas->drawColor(SK_ColorWHITE);

    SkPaint paint;
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(4);
    paint.setColor(SK_ColorRED);

    SkRect rect = SkRect::MakeXYWH(50, 50, 40, 60);
    canvas->drawRect(rect, paint);

    SkRRect oval;
    oval.setOval(rect);
    oval.offset(40, 60);
    paint.setColor(SK_ColorBLUE);
    canvas->drawRRect(oval, paint);

    paint.setColor(SK_ColorCYAN);
    canvas->drawCircle(180, 50, 25, paint);

    rect.offset(80, 0);
    paint.setColor(SK_ColorYELLOW);
    canvas->drawRoundRect(rect, 10, 10, paint);

    SkPath path;
    path.cubicTo(768, 0, -512, 256, 256, 256);
    paint.setColor(SK_ColorGREEN);
    //canvas->drawPath(path, paint);
    drawPathWithCircularBrush( canvas, path, 10, 4);

    //canvas->drawImage(image, 128, 128, SkSamplingOptions(), &paint);

    SkRect rect2 = SkRect::MakeXYWH(0, 0, 40, 60);
    //canvas->drawImageRect(image, rect2, SkSamplingOptions(), &paint);

    SkPaint paint2;
    auto text = SkTextBlob::MakeFromString("Hello, Skia!", SkFont(nullptr, 18));
    canvas->drawTextBlob(text.get(), 50, 25, paint2);
}


const int kWidth = 960;
const int kHeight = 640;

int main(int argc, char **argv) {
    // Initialize GLFW
	glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return -1;

    // Create OpenGL window
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Skia with OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Setup ImGui
    setupImGui(window);

    // Skia Initialization
	init_skia(kWidth, kHeight);

	glfwSwapInterval(1);
	glfwSetKeyCallback(window, key_callback);

	// Draw to the surface via its SkCanvas.
	SkCanvas* canvas = sSurface->getCanvas(); // We don't manage this pointer's lifetime.


    while (!glfwWindowShouldClose(window)) {

        glfwPollEvents();

        // Tell OpenGL a new frame is about to begin
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        renderImGui(); // some UI

        // Skia
        SkPaint paint;
		paint.setColor(SK_ColorWHITE);
		//canvas->drawPaint(paint);

		paint.setColor(SK_ColorBLUE);
		//canvas->drawRect({100, 200, 300, 500}, paint);

        //drawCircleWithNumber( canvas, SkPoint::Make( 400, 500), 50, 25);
        drawExample( canvas);

        // Implot demo
        ImPlot::ShowDemoWindow();

        ///
        sContext->flush();              // draw skia
        ImGui::Render();                // render imgui
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
