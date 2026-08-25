ABSTRACT

The State of Health (SoH) of a lithium-ion battery is a key indicator of its aging and remaining life. Accurate estimation of SoH helps prevent unexpected battery failures, optimize maintenance schedules, and extend operational lifespan, particularly in electric vehicles and energy storage applications. This project presents a hybrid deep learning framework combining Convolutional Neural Networks (CNNs) and Gated Recurrent Units (GRUs) for SoH prediction using NASA battery datasets (B0005 , B0018). The CNN component captures spatial dependencies within cycle-based discharge data, while the GRU component models temporal degradation behavior across cycles.

The proposed CNN–GRU model achieved an RMSE of 0.000113 and R² = 0.999998 on the B0005 dataset.. The model architecture was inspired by the CART–GX hybrid framework described in “Optimizing Battery Health Monitoring in Electric Vehicles Using Interpretable CART–GX Model” (Results in Engineering, 2025) and was further enhanced with structural simplifications based on “Real-Time Prediction Method of Remaining Useful Life Based on TinyML” (IEEE RCAR, 2022).
To improve computational efficiency, the trained CNN–GRU model underwent unstructured and structured pruning followed by fine-tuning, as well as dynamic quantization analysis. These optimization techniques significantly reduced model parameters and latency while preserving predictive accuracy (R² > 0.94). The results demonstrate that the CNN–GRU architecture provides an accurate and computationally efficient solution for real-time battery health estimation suitable for integration into modern battery management systems.

And the proposed ML model is uploaded in raspberry pi 5 and tested using real_time dataset from 18650 Lithium-ion battery with capacity of 2.6Ah with 3.7V nominal voltage.


BATTERY DATASET details

Source: NASA Ames Prognostics Center of Excellence (PCoE)

Experiment: Li-ion 18650 cells cycled to failure under different loads and temperatures.

Goal: Predict battery health metrics — Capacity, State of Health (SoH)

GENERAL DATASET OVERVIEW

Dataset 	Total Samples	   Unique Discharge Cycles     	Avg. Samples per Cycle     	Data Type

 B0005     	50,285         49,115 after cleaning             168	~290	          Discharge-only

 B0018	    34,866         33,948 after cleaning             132	~257	          Discharge-only

DESIGN SPECIFICATIONS

<img width="816" height="805" alt="image" src="https://github.com/user-attachments/assets/a434b00d-3f22-4d9a-be47-9490c28f194e" />

OUTPUT
<img width="874" height="432" alt="image" src="https://github.com/user-attachments/assets/2616f3ac-48ff-42b8-85a1-babd39c57af6" />

