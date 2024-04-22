#include "Plot.h"

#include <imgui.h>
#include <implot.h>
#include <chrono>
#include <format>

#if defined WIN32
#define BUFFER_SIZE 12'000'000
#else
#define BUFFER_SIZE 100'000'000
#endif

#pragma region Constructors/Destructor
fro_GENERATED_GUI_CONSTRUCTOR(Plot)
	, m_vIntegers(BUFFER_SIZE)
	, m_vGameObjects3D(BUFFER_SIZE)
	, m_vGameObjects3DAlt(BUFFER_SIZE)
{
}

fro_GENERATED_GUI_DESTRUCTOR(Plot)
{
}
#pragma endregion Constructors/Destructor



#pragma region PrivateMethods
fro_GENERATED_GUI_DISPLAY(Plot)
{
	constexpr double aStepSizes[]{ 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024 };
	constexpr int amountOfStepSizes{ 11 };

	ImGui::Begin("Exercise 1");
	ImGui::SliderInt("Amount Of Samples", &m_AmountOfSamplesInteger, 10, 100);

	if (ImGui::Button("Trash the cache"))
	m_vIntegersResults = measure(m_vIntegers);

	if (!m_vIntegersResults.empty() && ImPlot::BeginPlot("Integers"))
	{
		ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
		ImPlot::SetupAxisTicks(ImAxis_X1, aStepSizes, amountOfStepSizes);
		ImPlot::PlotLine("", aStepSizes, m_vIntegersResults.data(), amountOfStepSizes);
	
		ImPlot::EndPlot();
	}

	ImGui::End();

	ImGui::Begin("Exercise 2");
	ImGui::SliderInt("Amount Of Samples", &m_AmountOfSamples, 10, 100);

	if (ImGui::Button("Trash the cache"))
	{
		m_vGameObject3DResults = measure(m_vGameObjects3D);
		m_vGameObject3DAltResults = measure(m_vGameObjects3DAlt);
	}

	if (!m_vGameObject3DResults.empty() && !m_vGameObject3DAltResults.empty())
	{
		if (ImPlot::BeginPlot("GameObject3D"))
		{
			ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
			ImPlot::SetupAxisTicks(ImAxis_X1, aStepSizes, amountOfStepSizes);
			ImPlot::PlotLine("", aStepSizes, m_vGameObject3DResults.data(), amountOfStepSizes);
	
			ImPlot::EndPlot();
		}

		if (ImPlot::BeginPlot("GameObject3DAlt"))
		{
			ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
			ImPlot::SetupAxisTicks(ImAxis_X1, aStepSizes, amountOfStepSizes);
			ImPlot::PlotLine("", aStepSizes, m_vGameObject3DAltResults.data(), amountOfStepSizes);
	
			ImPlot::EndPlot();
		}

		if (ImPlot::BeginPlot("GameObject3D vs GameObject3DAlt"))
		{
			ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Log10);
			ImPlot::SetupAxisTicks(ImAxis_X1, aStepSizes, amountOfStepSizes);
			ImPlot::PlotLine("GameObject3D", aStepSizes, m_vGameObject3DResults.data(), amountOfStepSizes);
			ImPlot::PlotLine("GameObject3DAlt", aStepSizes, m_vGameObject3DAltResults.data(), amountOfStepSizes);
	
			ImPlot::EndPlot();
		}
	}

	ImGui::End();
}
#pragma endregion PrivateMethods
#undef BUFFER_SIZE